/*
 * Exercise 9.2: Core file generation and debugging
 * Generates a segmentation fault and demonstrates core file creation
 */
#include <stdio.h>
#include <stdlib.h>

/* Global array of 10 integers */
int global_array[10];

/* Function b - writes beyond array bounds to cause segfault */
void b(void) {
    int i;
    /* Write to array from index 0 without upper bound - this will cause segfault */
    for (i = 0; ; i++) {
        global_array[i] = i;  /* This will eventually write beyond array bounds */
    }
}

/* Function a - calls function b */
void a(void) {
    b();
}

/* Main function - calls function a */
int main(void) {
    printf("Starting program - this will cause a segmentation fault\n");
    printf("Make sure to compile with: gcc -g -o ex9_2 ex9_2.c\n");
    printf("And run with: ulimit -c unlimited\n");
    
    a();
    
    /* This will never be reached due to segfault */
    printf("This line will never be printed\n");
    
    return 0;
}