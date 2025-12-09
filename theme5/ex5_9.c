/*
 * Exercice 5.9
 * We want to implement a new version of the standard I/O library using system primitives.
 * 
 * 1. Give a definition of the FILE type. Don't forget to plan for input/output buffering.
 * 2. Program the my_open function, analogous to fopen. To simplify, we only consider
 *    the "r" and "w" opening modes.
 * 3. Resume exercise 5.6 to program my_getc, analogous to getc.
 * 4. Program the my_putc function, analogous to putc (which buffers output the same
 *    way that getc buffers input).
 * 5. Program the my_close function, analogous to fclose.
 * 6. For testing, write a main function that opens two files toto and tata in reading,
 *    then reads in a loop 1 character in each of the two files and displays them on
 *    standard output. Your loop will stop as soon as you encounter the first end of file.
 */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#define BUFFER_SIZE 8192

/* FILE structure definition using system primitives */
typedef struct {
    int fd;                    /* File descriptor */
    char *buffer;              /* I/O buffer */
    size_t buffer_size;        /* Buffer size */
    size_t buffer_pos;         /* Current position in buffer */
    size_t buffer_count;       /* Number of bytes in buffer */
    int mode;                  /* 0 = not open, 1 = read, 2 = write */
    int error;                 /* Error flag */
    int eof;                   /* EOF flag */
} MY_FILE;

/* Global stdin/stdout/stderr equivalents */
MY_FILE *my_stdin;
MY_FILE *my_stdout;
MY_FILE *my_stderr;

/* Initialize MY_FILE structure */
MY_FILE* my_fopen_alloc(void) {
    MY_FILE *fp = malloc(sizeof(MY_FILE));
    if (!fp) return NULL;
    
    fp->fd = -1;
    fp->buffer = malloc(BUFFER_SIZE);
    if (!fp->buffer) {
        free(fp);
        return NULL;
    }
    fp->buffer_size = BUFFER_SIZE;
    fp->buffer_pos = 0;
    fp->buffer_count = 0;
    fp->mode = 0;
    fp->error = 0;
    fp->eof = 0;
    
    return fp;
}

/* Free MY_FILE structure */
void my_fclose_free(MY_FILE *fp) {
    if (fp) {
        if (fp->buffer) free(fp->buffer);
        free(fp);
    }
}

/* Open file - analogous to fopen */
MY_FILE* my_open(const char *pathname, const char *mode) {
    MY_FILE *fp = my_fopen_alloc();
    if (!fp) return NULL;
    
    if (strcmp(mode, "r") == 0) {
        fp->fd = open(pathname, O_RDONLY);
        fp->mode = 1;  /* read mode */
    } else if (strcmp(mode, "w") == 0) {
        fp->fd = open(pathname, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        fp->mode = 2;  /* write mode */
    } else {
        my_fclose_free(fp);
        errno = EINVAL;
        return NULL;
    }
    
    if (fp->fd == -1) {
        my_fclose_free(fp);
        return NULL;
    }
    
    return fp;
}

/* Fill read buffer */
static int fill_buffer(MY_FILE *fp) {
    if (fp->mode != 1) return 0;  /* not in read mode */
    
    fp->buffer_pos = 0;
    fp->buffer_count = read(fp->fd, fp->buffer, fp->buffer_size);
    
    if (fp->buffer_count == -1) {
        fp->error = 1;
        return 0;
    }
    
    if (fp->buffer_count == 0) {
        fp->eof = 1;
        return 0;
    }
    
    return 1;
}

/* Get character - analogous to getc */
int my_getc(MY_FILE *fp) {
    if (fp->mode != 1) {  /* not in read mode */
        fp->error = 1;
        return -1;
    }
    
    /* If buffer is empty, fill it */
    if (fp->buffer_pos >= fp->buffer_count) {
        if (!fill_buffer(fp)) {
            return -1;  /* error or EOF */
        }
    }
    
    /* Return next character from buffer */
    return (unsigned char)fp->buffer[fp->buffer_pos++];
}

/* Flush write buffer */
static int flush_buffer(MY_FILE *fp) {
    if (fp->mode != 2) return 0;  /* not in write mode */
    
    if (fp->buffer_pos > 0) {
        ssize_t written = write(fp->fd, fp->buffer, fp->buffer_pos);
        if (written != (ssize_t)fp->buffer_pos) {
            fp->error = 1;
            return 0;
        }
        fp->buffer_pos = 0;
    }
    
    return 1;
}

/* Put character - analogous to putc */
int my_putc(int c, MY_FILE *fp) {
    if (fp->mode != 2) {  /* not in write mode */
        fp->error = 1;
        return -1;
    }
    
    /* If buffer is full, flush it */
    if (fp->buffer_pos >= fp->buffer_size) {
        if (!flush_buffer(fp)) {
            return -1;
        }
    }
    
    /* Add character to buffer */
    fp->buffer[fp->buffer_pos++] = (char)c;
    
    return c;
}

/* Close file - analogous to fclose */
int my_close(MY_FILE *fp) {
    int result = 0;
    
    if (!fp) return -1;
    
    /* Flush write buffer if in write mode */
    if (fp->mode == 2) {
        if (!flush_buffer(fp)) {
            result = -1;
        }
    }
    
    /* Close file descriptor */
    if (fp->fd != -1) {
        if (close(fp->fd) == -1) {
            result = -1;
        }
    }
    
    /* Free structure */
    my_fclose_free(fp);
    
    return result;
}

/* Check for errors */
int my_ferror(MY_FILE *fp) {
    return fp && fp->error;
}

/* Check for EOF */
int my_feof(MY_FILE *fp) {
    return fp && fp->eof;
}

/* Get character from stdin - analogous to getchar */
int my_getchar(void) {
    return my_getc(my_stdin);
}

/* Put character to stdout - analogous to putchar */
int my_putchar(int c) {
    return my_putc(c, my_stdout);
}

/* Initialize standard streams */
void my_init_standard_streams(void) {
    /* Create stdin */
    my_stdin = my_fopen_alloc();
    if (my_stdin) {
        my_stdin->fd = STDIN_FILENO;
        my_stdin->mode = 1;
    }
    
    /* Create stdout */
    my_stdout = my_fopen_alloc();
    if (my_stdout) {
        my_stdout->fd = STDOUT_FILENO;
        my_stdout->mode = 2;
    }
    
    /* Create stderr */
    my_stderr = my_fopen_alloc();
    if (my_stderr) {
        my_stderr->fd = STDERR_FILENO;
        my_stderr->mode = 2;
    }
}

/* Free standard streams */
void my_close_standard_streams(void) {
    /* Don't close standard file descriptors, just free structures */
    if (my_stdin) {
        my_stdin->fd = -1;  /* prevent close() from being called */
        my_fclose_free(my_stdin);
    }
    if (my_stdout) {
        my_stdout->fd = -1;  /* prevent close() from being called */
        my_fclose_free(my_stdout);
    }
    if (my_stderr) {
        my_stderr->fd = -1;  /* prevent close() from being called */
        my_fclose_free(my_stderr);
    }
}

/* Test program */
int main(void) {
    MY_FILE *file1, *file2;
    int c1, c2;
    
    printf("Testing my I/O library implementation\n");
    printf("=====================================\n\n");
    
    /* Initialize standard streams */
    my_init_standard_streams();
    
    /* Open test files (create them first if they don't exist) */
    printf("Opening files 'toto' and 'tata' for reading...\n");
    file1 = my_open("toto", "r");
    file2 = my_open("tata", "r");
    
    if (!file1) {
        fprintf(stderr, "Error opening file 'toto'\n");
        perror("toto");
    }
    
    if (!file2) {
        fprintf(stderr, "Error opening file 'tata'\n");
        perror("tata");
    }
    
    if (file1 && file2) {
        printf("Files opened successfully. Reading characters alternately:\n");
        printf("----------------------------------------------------------\n");
        
        /* Read alternately from both files */
        int line_count = 1;
        while (!my_feof(file1) && !my_feof(file2)) {
            c1 = my_getc(file1);
            c2 = my_getc(file2);
            
            if (!my_feof(file1)) {
                printf("toto[%d]: %c", line_count, c1);
                if (c1 == '\n') line_count++;
            }
            
            if (!my_feof(file2)) {
                printf("  tata[%d]: %c", line_count, c2);
                if (c2 == '\n') line_count++;
            }
            
            printf("\n");
            
            /* Stop if either file reaches EOF */
            if (my_feof(file1) || my_feof(file2)) {
                break;
            }
        }
        
        printf("\nReading completed.\n");
    }
    
    /* Close files */
    if (file1) my_close(file1);
    if (file2) my_close(file2);
    
    /* Clean up standard streams */
    my_close_standard_streams();
    
    printf("Test completed.\n");
    return 0;
}

/*
 * IMPLEMENTATION NOTES:
 * 
 * 1. FILE structure:
 *    - Uses file descriptor instead of FILE* from standard library
 *    - Implements buffering with configurable buffer size
 *    - Tracks position within buffer for efficient access
 *    - Separates read and write modes
 * 
 * 2. Buffering strategy:
 *    - Read: Fills buffer when empty, returns characters from buffer
 *    - Write: Accumulates in buffer, flushes when full or on close
 *    - Automatic flushing on file close for write mode
 * 
 * 3. Error handling:
 *    - Separate error and EOF flags
 *    - Error propagation through return values
 *    - Proper cleanup on errors
 * 
 * 4. Compatibility:
 *    - Uses same return values as standard functions
 *    - EOF constant for end-of-file
 *    - Similar function prototypes
 * 
 * This demonstrates how the standard C I/O library is built on top
 * of system primitives like read(), write(), open(), and close().
 */