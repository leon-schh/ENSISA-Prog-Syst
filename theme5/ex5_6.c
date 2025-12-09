/*
 * Exercice 5.6
 * Write a buffered version of getchar. For this, your version must keep a buffer in memory.
 * If the buffer is empty when your getchar is called, it must fill the buffer and return the first byte.
 * If the buffer already contains bytes, your getchar must return the next byte.
 */

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>

#define BUFFER_SIZE 8192

/* Static buffer and position tracking */
static char input_buffer[BUFFER_SIZE];
static size_t buffer_pos = 0;
static size_t buffer_count = 0;
static int initialized = 0;

/* Initialize the buffer */
static void init_buffer(void) {
    if (!initialized) {
        buffer_pos = 0;
        buffer_count = 0;
        initialized = 1;
    }
}

/* Buffered getchar implementation */
int buffered_getchar(void) {
    char c;
    
    /* Initialize buffer on first call */
    init_buffer();
    
    /* If buffer is empty, fill it */
    if (buffer_pos >= buffer_count) {
        ssize_t bytes_read = read(STDIN_FILENO, input_buffer, BUFFER_SIZE);
        
        if (bytes_read <= 0) {
            /* Error or EOF */
            return (bytes_read == 0) ? EOF : -1;
        }
        
        buffer_count = bytes_read;
        buffer_pos = 0;
    }
    
    /* Return next character from buffer */
    c = input_buffer[buffer_pos++];
    return (unsigned char)c;
}

/* Reset buffer state (useful for testing) */
void reset_buffered_getchar(void) {
    initialized = 0;
    buffer_pos = 0;
    buffer_count = 0;
}

/* Get buffer status for debugging */
void get_buffer_status(size_t *pos, size_t *count, int *init) {
    if (pos) *pos = buffer_pos;
    if (count) *count = buffer_count;
    if (init) *init = initialized;
}

/* Test function: copy stdin to stdout using buffered getchar */
void test_buffered_copy(void) {
    int c;
    long total_chars = 0;
    
    printf("Testing buffered getchar...\n");
    printf("Buffer size: %d bytes\n", BUFFER_SIZE);
    
    while ((c = buffered_getchar()) != EOF) {
        if (putchar(c) == EOF) {
            fprintf(stderr, "Error writing to stdout\n");
            break;
        }
        total_chars++;
    }
    
    printf("\nTotal characters processed: %ld\n", total_chars);
}

/* Demonstration of buffer efficiency */
void demonstrate_buffering(void) {
    int c;
    size_t pos, count, init;
    
    printf("Demonstrating buffer behavior:\n");
    printf("Reading first few characters with buffering:\n");
    
    /* Reset to start fresh */
    reset_buffered_getchar();
    
    for (int i = 0; i < 10; i++) {
        c = buffered_getchar();
        if (c == EOF) break;
        
        get_buffer_status(&pos, &count, &init);
        printf("Char %d: '%c' (buffer: pos=%zu, count=%zu, init=%d)\n", 
               i+1, (c >= 32 && c < 127) ? c : '?', pos, count, init);
    }
}

int main(void) {
    struct stat st;
    
    /* Get info about stdin */
    if (fstat(STDIN_FILENO, &st) == 0 && S_ISREG(st.st_mode)) {
        printf("Input file size: %ld bytes\n", (long)st.st_size);
    } else {
        printf("Input is not a regular file\n");
    }
    
    printf("\n=== Testing buffered getchar ===\n");
    test_buffered_copy();
    
    printf("\n=== Buffer demonstration ===\n");
    demonstrate_buffering();
    
    printf("\nBuffered getchar test completed!\n");
    return 0;
}

/*
 * BUFFERING BENEFITS:
 * 
 * 1. EFFICIENCY: Reduces system calls by reading multiple bytes at once
 * 2. PERFORMANCE: Fewer context switches between user and kernel space
 * 3. THROUGHPUT: Better performance for large files or streams
 * 
 * COMPARISON WITH UNBUFFERED:
 * - Unbuffered: 1 system call per character
 * - Buffered: 1 system call per BUFFER_SIZE characters
 * - For large files, buffered version is much faster
 * 
 * This is similar to how the standard library getchar() works,
 * but implemented using system primitives directly.
 */