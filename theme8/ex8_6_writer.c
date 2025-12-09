#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>

#define BUFFER_SIZE 4096
#define FIFO_NAME "mon_tube_nomme"

/**
 * Copy data from source file descriptor to destination file descriptor
 */
void copier(int fdsrc, int fddst) {
    char buffer[BUFFER_SIZE];
    ssize_t bytes_read;
    
    while ((bytes_read = read(fdsrc, buffer, BUFFER_SIZE)) > 0) {
        ssize_t bytes_written = 0;
        ssize_t total_written = 0;
        
        while (total_written < bytes_read) {
            bytes_written = write(fddst, buffer + total_written, bytes_read - total_written);
            if (bytes_written < 0) {
                perror("Erreur lors de l'écriture");
                return;
            }
            total_written += bytes_written;
        }
    }
    
    if (bytes_read < 0) {
        perror("Erreur lors de la lecture");
    }
}

/**
 * Exercise 8.6 - Writer part: Creates named pipe and writes data from stdin
 */
int main(void) {
    int fd;
    
    // Remove existing named pipe if it exists
    unlink(FIFO_NAME);
    
    // Create named pipe with permissions rw-rw-rw-
    if (mkfifo(FIFO_NAME, 0666) == -1) {
        perror("mkfifo");
        exit(EXIT_FAILURE);
    }
    
    printf("Tube nommé '%s' créé. En attente du lecteur...\n", FIFO_NAME);
    
    // Open named pipe for writing
    fd = open(FIFO_NAME, O_WRONLY);
    if (fd == -1) {
        perror("open");
        unlink(FIFO_NAME);
        exit(EXIT_FAILURE);
    }
    
    printf("Tube ouvert. Lecture depuis stdin et écriture dans le tube...\n");
    
    // Read from stdin and write to named pipe
    copier(STDIN_FILENO, fd);
    
    printf("Transfert terminé.\n");
    
    close(fd);
    unlink(FIFO_NAME);
    
    return 0;
}