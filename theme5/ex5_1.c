/*
 * Exercice 5.1
 * Questions about system primitives
 * 
 * This file contains answers to theoretical questions about system primitives
 * and their differences from library functions.
 */

#include <stdio.h>

/*
 * Q1: Name some system primitives. Is fopen a system primitive?
 * 
 * System primitives (also called system calls) include:
 * - open, read, write, close (file operations)
 * - fork, exec, wait (process management)
 * - pipe, dup, dup2 (I/O redirection)
 * - mkdir, rmdir (directory operations)
 * - chmod, chown (file permissions)
 * - stat, fstat, lstat (file information)
 * 
 * fopen is NOT a system primitive - it's a library function from stdio.h
 * that provides a higher-level interface to file operations.
 */

/*
 * Q2: What are the differences and similarities between system primitives 
 * and library functions? How to justify these differences?
 * 
 * Differences:
 * - System primitives are direct interfaces to the operating system kernel
 * - Library functions are higher-level abstractions built on top of system primitives
 * - System primitives have more error cases to handle
 * - Library functions provide buffering and convenience features
 * - System primitives are more portable across different Unix-like systems
 * - Library functions may have additional platform-specific optimizations
 * 
 * Similarities:
 * - Both provide file I/O functionality
 * - Both can handle the same types of files (regular files, devices, etc.)
 * - Both return similar error codes (typically -1 on error)
 * 
 * Justification for differences:
 * - System primitives need to be minimal and direct to reduce kernel overhead
 * - Library functions can provide conveniences like automatic buffering
 * - Different applications may need different levels of control vs convenience
 * - Error handling complexity is better managed in user space (library functions)
 */

/*
 * Q3: Illustrate these differences and similarities on I/O functions and primitives
 * 
 * System primitives for I/O:
 * - open(), read(), write(), close()
 * - Direct kernel interface, no buffering
 * - Must handle all error cases explicitly
 * - Fixed buffer sizes, no automatic management
 * 
 * Library functions for I/O:
 * - fopen(), fread(), fwrite(), fclose()
 * - Built on top of system primitives with added features
 * - Automatic buffering, line ending translation
 * - Formatted I/O (fprintf, fscanf)
 * - Better error handling withferror(), feof()
 * 
 * Example: Reading a character
 * System primitive way:
 *   char c;
 *   if (read(fd, &c, 1) != 1) handle_error();
 * 
 * Library function way:
 *   int c = fgetc(fp);
 *   if (c == EOF) handle_error();
 * 
 * The library function provides automatic buffering, better error reporting,
 * and portable behavior across different systems.
 */

int main(void) {
    printf("Exercice 5.1 - Theoretical questions about system primitives\n");
    printf("See comments in the source code for detailed answers.\n");
    return 0;
}