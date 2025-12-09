/*
 * Exercice 5.5
 * Write the getchar function that returns a character read from standard input,
 * or the EOF constant at end of file.
 * 
 * What can be the numeric value of the EOF constant?
 * 
 * For testing, write a main function that copies all characters read from
 * standard input to standard output using your function and the putchar function.
 * Use your program by redirecting standard input from a large file, preferably
 * binary (a program in /bin for example), and standard output to a new file.
 * Use the cmp command to compare the original and your program's output:
 * if cmp displays nothing, the two files are identical.
 */

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

/* Our implementation of getchar using system primitives */
int my_getchar(void) {
    char c;
    ssize_t result = read(STDIN_FILENO, &c, 1);
    
    if (result == 1) {
        return (unsigned char)c;
    } else if (result == 0) {
        return EOF;  /* End of file */
    } else {
        return EOF;  /* Error - treat as EOF for compatibility */
    }
}

/* Our implementation of putchar for completeness */
int my_putchar(int c) {
    char ch = (char)c;
    ssize_t result = write(STDOUT_FILENO, &ch, 1);
    
    if (result == 1) {
        return c;
    } else {
        return EOF;
    }
}

/* Copy stdin to stdout using our functions */
void copy_stdin_to_stdout(void) {
    int c;
    
    printf("Starting copy (my_getchar -> my_putchar)...\n");
    
    while ((c = my_getchar()) != EOF) {
        if (my_putchar(c) == EOF) {
            fprintf(stderr, "Error writing to stdout\n");
            break;
        }
    }
    
    printf("Copy completed.\n");
}

int main(void) {
    struct stat st;
    long original_size = 0;
    
    /* Get size of stdin if it's a regular file */
    if (fstat(STDIN_FILENO, &st) == 0 && S_ISREG(st.st_mode)) {
        original_size = st.st_size;
        printf("Input is a regular file, size: %ld bytes\n", original_size);
    } else {
        printf("Input is not a regular file (pipe, terminal, etc.)\n");
    }
    
    /* Copy stdin to stdout */
    copy_stdin_to_stdout();
    
    /* Get size of stdout if it's a regular file */
    if (fstat(STDOUT_FILENO, &st) == 0 && S_ISREG(st.st_mode)) {
        printf("Output file size: %ld bytes\n", st.st_size);
        if (original_size > 0 && st.st_size == original_size) {
            printf("Sizes match! Copy was successful.\n");
        }
    }
    
    printf("\nEOF constant value: %d\n", EOF);
    printf("Note: EOF is typically -1 on most systems\n");
    
    return 0;
}

/*
 * ANSWER TO THEORETICAL QUESTION:
 * 
 * What can be the numeric value of the EOF constant?
 * 
 * The EOF constant is typically defined as -1 in most C implementations.
 * This is because:
 * 1. It needs to be different from any valid character value (0-255 for unsigned char)
 * 2. It needs to fit in an int (character functions return int, not char)
 * 3. -1 is a convenient choice as it's easily distinguishable from character values
 * 
 * The exact value is implementation-defined, but -1 is the most common choice.
 * It must be a negative integer that cannot be confused with a character value.
 */