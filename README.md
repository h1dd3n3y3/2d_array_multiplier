# 2D-array Multiplier
This is a UNIX OS intended program, that multiplies 2 2D-arrays and displays the output array to your terminal screen.

## Compile
  * ### Method 1 (easiest):
    Use the Makefile provided by cd-ing to the project directory and hitting `make` command.  
  * ### Method 2 (beware the linker):
    `gcc 2d_arr_mul.c -o 2d_arr_mul -lm`  
    "-lm" helps the compiler to link the `floor()` and `log10()` functions to math.h library.  