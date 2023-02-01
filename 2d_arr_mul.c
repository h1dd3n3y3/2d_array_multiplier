#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h> //
#include <math.h> // floor(), log10(): gcc must contain the "-lm" argument

#define rows1 atoi(argv[1]) //
#define cols1 atoi(argv[2]) // atoi(const char* str) function converts string to integer values
#define rows2 atoi(argv[3]) // argv array (command line arguments) is type of char** (aka array of strings(array of characters))
#define cols2 atoi(argv[4]) //

void usage(char *exe) { // Display usage message
    printf("[!]Usage: '%s n m m k'\n", exe);
    printf("   Where:\n");
    printf("\t1. (n x m) & (m x k): 1st & 2nd array's dimentions (rows x columns).\n");
    printf("\t2. 'm' indicates that: '1st array's columns == 2nd array's rows'.\n");
}

void initArr(int rows, int cols, int arr[rows][cols]) { // Initialize the given array
    for (int r = 0; r < rows; r++)
        for (int c = 0; c < cols; c++)
            arr[r][c] = (rand() % 20) + 1;
}

int getMaxDigitLen(int rows, int cols, int arr[rows][cols]) { // Find array's number with the most digits
    int maxDigitLen = floor(log10(arr[0][0])) + 1; // Assume the first array number has the most digits
    int newDigitLen;

    // Sweep all array and find the number with the most digits
    for (int r = 0; r < rows; r++)
        for (int c = 0; c < cols; c++) {
            newDigitLen = floor(log10(arr[r][c])) + 1; // Calculate each number's digit number
            if (newDigitLen > maxDigitLen) maxDigitLen = newDigitLen;
        }

    return maxDigitLen;
}

void printArr(int rows, int cols, int arr[rows][cols], char* name) { // Print the given array
    int maxDigitLen = getMaxDigitLen(rows, cols, arr);
    int currDigitLen;

    // Text decoration before first row
    printf("%s =\n\t  ┌─", name);
    for (int s = 0; s < (maxDigitLen + 2) * cols - 2; s++) printf(" ");
    printf("─┐\n");

    for (int r = 0; r < rows; r++)
        for (int c = 0; c < cols; c++) {
            currDigitLen = floor(log10(arr[r][c])) + 1; // Calculate each number's digit number
            // Text decoration in the beginning of each column
            if (c == 0) printf("\t  │ ");
            // Fill with spaces in front of digit to match array's max digit length
            for (int i = currDigitLen; i < maxDigitLen; i++) printf(" ");
            // Print the number
            printf("%d  ", arr[r][c]);
            // Text decoration in the end of each column
            if (c == cols - 1) printf("\b│\n");
        }
    // Text decoration after last row
    printf("\t  └─");
    for (int s = 0; s < (maxDigitLen + 2) * cols - 2; s++) printf(" ");
    printf("─┘ (%d × %d)\n", rows, cols);
}

void sysCallFail(char *str, int code) { // Print error message and exit
    fprintf(stderr, "[-]%s.\n", str); exit(code);
}

int main(int argc, char **argv) {
    if (argc != 5) { // Check command line argument number, including the executable itself
        fprintf(stderr, "[-]Not many args\n");
        usage(argv[0]);
        return 1;
    } else if (cols1 != rows2) { // Check if array multiplication rules are not being followed
        fprintf(stderr, "[-]1st array's columns != 2nd array's rows.\n");
        usage(argv[0]);
        return 2;
    }

    pid_t pid;
    int arr1[rows1][cols1], arr2[rows2][cols2], arr3[rows1][cols2];
    int fd_row[2], fd_col[2], fd_res[2];

    srand(time(0)); // Initialize srand() properely with the time of OS startup till the program runs, so rand() can generate different results every time called

    // Initialize and print 1st array
    initArr(rows1, cols1, arr1);
    printArr(rows1, cols1, arr1, "A");
    
    // Initialize and print 2nd array
    initArr(rows2, cols2, arr2);
    printArr(rows2, cols2, arr2, "B");

    for (int r1 = 0; r1 < rows1; r1++) // Loop through 1st array rows (1st array's rows)
        for (int c2 = 0; c2 < cols2; c2++) { // Loop through 2nd array columns (2nd array's columns)
            // Create and check pipe outputs
            if (pipe(fd_row) == -1) sysCallFail("Pipe failure", 1);
            if (pipe(fd_col) == -1) sysCallFail("Pipe failure", 2);
            if (pipe(fd_res) == -1) sysCallFail("Pipe failure", 3);

            if ((pid = fork()) > 0) { // Parent process
                int res_p;

                close(fd_row[0]); // Close unnecessary pipe end for "read"
                if (write(fd_row[1], &r1, sizeof(r1)) == -1) sysCallFail("Write failure", 5); // Check and write the array row to "write" pipe end
                close(fd_row[1]); // Close "write" pipe end when finished
                
                close(fd_col[0]); // Close unnecessary pipe end for "read"
                if (write(fd_col[1], &c2, sizeof(c2)) == -1) sysCallFail("Write failure", 6); // Check and write the array column to "write" pipe end
                close(fd_col[1]); // Close "write" pipe end when finished
                
                close(fd_res[1]); // Close unnecessary pipe end for "write"
                if (read(fd_res[0], &res_p, sizeof(res_p)) == -1) sysCallFail("Read failure", 7); // Check and read the array row from "read" pipe end and save it
                close(fd_res[0]); // Close "write" pipe end when finished
                
                arr3[r1][c2] = res_p; // Save the read result to the new array
            } else if (pid == 0) { // Child process
                int row1, col2, res_c = 0;

                close(fd_row[1]); // Close unnecessary pipe end for "write"
                if (read(fd_row[0], &row1, sizeof(row1)) == -1) sysCallFail("Read failure", 5); // Check and read the array row from "read" pipe end and save it
                close(fd_row[0]); // Close "write" pipe end when finished

                close(fd_col[1]); // Close unnecessary pipe end for "write"
                if (read(fd_col[0], &col2, sizeof(col2)) == -1) sysCallFail("Read failure", 6); // Check and read the array column from "read" pipe end and save it
                close(fd_col[0]); // Close "write" pipe end when finished

                for (int k = 0; k < cols1; k++) // Repeat for all row/column numbers (1st table rows == 2nd table columns)
                    res_c += arr1[row1][k] * arr2[k][col2]; // Multiply each number by moving to the next row/column item each time

                close(fd_res[0]); // Close unnecessary pipe end for "read"
                if (write(fd_res[1], &res_c, sizeof(res_c)) == -1) sysCallFail("Write failure", 7); // Check and write the result to "write" pipe end
                close(fd_res[1]); // Close "write" pipe end when finished

                exit(0); // Successfully (code 0) terminate child process, when finished, avoiding zombified process creations
            } else { // If pid < 0 then fork failed
                sysCallFail("fork failure", 4);
            }
        }
    // Print the final array
    printArr(rows1, cols2, arr3, "A × B");

    return 0;
}