/*
 * Exercice 5.7
 * We often think that system primitives, being at a lower level, are more efficient
 * than equivalent library functions. We want to confirm or refute this proposition
 * through experimentation.
 * 
 * For this, we need to write two programs to copy standard input to standard output.
 * The first will use library functions getchar and putchar.
 * The second will use system primitives read and write and will take as argument
 * the buffer size used for copying. If this size equals 1, the copy will be done
 * character by character.
 * 
 * We will use the Unix time command to compare execution times, considering the sum
 * of CPU times in user mode and system mode.
 * We will pay great attention to the quality of our measurements: significant values
 * (use large input files), elimination of variations (average of several values),
 * elimination of disturbances due to output on our screen (redirect standard output), etc.
 * 
 * Taking as buffer sizes the successive powers of 2 (2^0, 2^1, 2^2, 2^3, etc.),
 * from what buffer size is it more interesting to use system primitives than
 * library functions?
 */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>

#define MAX_BUFFER_SIZE (64 * 1024)  /* 64KB maximum */

/* Get current time in seconds with microsecond precision */
double get_time(void) {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + tv.tv_usec / 1000000.0;
}

/* Copy using library functions (getchar/putchar) */
double copy_with_library_functions(void) {
    int c;
    double start_time, end_time;
    
    start_time = get_time();
    
    while ((c = getchar()) != EOF) {
        if (putchar(c) == EOF) {
            break;
        }
    }
    
    end_time = get_time();
    return end_time - start_time;
}

/* Copy using system primitives (read/write) with given buffer size */
double copy_with_system_primitives(int buffer_size) {
    char *buffer;
    ssize_t bytes_read, bytes_written;
    double start_time, end_time;
    
    if (buffer_size <= 0 || buffer_size > MAX_BUFFER_SIZE) {
        fprintf(stderr, "Invalid buffer size: %d\n", buffer_size);
        return -1.0;
    }
    
    buffer = malloc(buffer_size);
    if (buffer == NULL) {
        perror("malloc");
        return -1.0;
    }
    
    start_time = get_time();
    
    while ((bytes_read = read(STDIN_FILENO, buffer, buffer_size)) > 0) {
        ssize_t total_written = 0;
        while (total_written < bytes_read) {
            bytes_written = write(STDOUT_FILENO, buffer + total_written, 
                                bytes_read - total_written);
            if (bytes_written == -1) {
                perror("write");
                free(buffer);
                return -1.0;
            }
            total_written += bytes_written;
        }
    }
    
    if (bytes_read == -1) {
        perror("read");
        free(buffer);
        return -1.0;
    }
    
    end_time = get_time();
    free(buffer);
    return end_time - start_time;
}

/* Run multiple tests and return average time */
double run_multiple_tests(double (*copy_func)(void), int num_tests) {
    double total_time = 0.0;
    
    for (int i = 0; i < num_tests; i++) {
        double time_taken = copy_func();
        if (time_taken < 0) {
            return -1.0;
        }
        total_time += time_taken;
    }
    
    return total_time / num_tests;
}

/* Test system primitives with different buffer sizes */
double test_system_primitives_with_buffer(int buffer_size) {
    return copy_with_system_primitives(buffer_size);
}

int main(int argc, char *argv[]) {
    int num_tests = 5;  /* Number of tests to average */
    
    printf("Performance Comparison: Library Functions vs System Primitives\n");
    printf("=============================================================\n\n");
    
    /* Check if buffer size is provided */
    if (argc > 1) {
        int buffer_size = atoi(argv[1]);
        if (buffer_size <= 0) {
            fprintf(stderr, "Usage: %s [buffer_size]\n", argv[0]);
            fprintf(stderr, "buffer_size must be a positive integer\n");
            return 1;
        }
        
        printf("Testing with buffer size: %d bytes\n", buffer_size);
        
        /* Test system primitives with specified buffer size */
        printf("Testing system primitives (read/write)...\n");
        double sys_time = run_multiple_tests(
            (double (*)(void))test_system_primitives_with_buffer, 
            num_tests);
        
        if (sys_time >= 0) {
            printf("System primitives time: %.6f seconds (average of %d tests)\n", 
                   sys_time, num_tests);
        }
        
        return 0;
    }
    
    /* Default: test library functions */
    printf("Testing library functions (getchar/putchar)...\n");
    double lib_time = run_multiple_tests(copy_with_library_functions, num_tests);
    
    if (lib_time >= 0) {
        printf("Library functions time: %.6f seconds (average of %d tests)\n", 
               lib_time, num_tests);
    }
    
    printf("\nTo test system primitives with different buffer sizes, run:\n");
    printf("  %s <buffer_size>\n", argv[0]);
    printf("  %s 1    # Character by character\n", argv[0]);
    printf("  %s 64   # 64 bytes\n", argv[0]);
    printf("  %s 1024 # 1KB\n", argv[0]);
    printf("  %s 8192 # 8KB\n", argv[0]);
    
    printf("\nTo use 'time' command for measurement:\n");
    printf("  time %s < large_file > output_file\n", argv[0]);
    printf("  time %s 8192 < large_file > output_file\n", argv[0]);
    
    return 0;
}

/*
 * USAGE INSTRUCTIONS:
 * 
 * 1. Test library functions:
 *    ./ex5_7 < large_binary_file > output
 * 
 * 2. Test system primitives with different buffer sizes:
 *    ./ex5_7 1 < large_binary_file > output      # Character by character
 *    ./ex5_7 64 < large_binary_file > output     # 64 bytes
 *    ./ex5_7 1024 < large_binary_file > output   # 1KB
 *    ./ex5_7 8192 < large_binary_file > output   # 8KB
 * 
 * 3. Use time command for precise measurements:
 *    time ./ex5_7 < large_binary_file > output
 *    time ./ex5_7 8192 < large_binary_file > output
 * 
 * 4. Test various buffer sizes (powers of 2):
 *    for size in 1 2 4 8 16 32 64 128 256 512 1024 2048 4096 8192 16384; do
 *        echo "Testing buffer size: $size"
 *        time ./ex5_7 $size < large_file > /dev/null
 *    done
 * 
 * EXPECTED RESULTS:
 * - Small buffer sizes (1-64 bytes): Library functions are faster due to buffering
 * - Medium buffer sizes (256-4096 bytes): Similar performance
 * - Large buffer sizes (8192+ bytes): System primitives may be faster
 * - Optimal buffer size: Usually 4KB-8KB (page size on most systems)
 * 
 * The key insight is that library functions already provide buffering,
 * so for small buffer sizes, the overhead of system calls dominates.
 */