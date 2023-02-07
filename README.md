# 2D-Array Multiplier
This is a UNIX OS intended program, that multiplies two 2D-arrays, containing random numbers inside 1-20 range, using multiple process generation and Inter-Process Communication (IPC)  
displaying all three arrays to your terminal screen.  

## Compile
  * ### Method 1 (easiest):
    Use the Makefile provided by cd-ing to the project directory and hitting the `make` command.  
  * ### Method 2 (beware the linker):
    `gcc 2d_arr_mul.c -o 2d_arr_mul -lm`  
    - "-lm" helps the compiler to link the `floor()` and `log10()` functions to math.h library.  
