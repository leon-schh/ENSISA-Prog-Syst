#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

#define BUFFER_SIZE 4096

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
 * Exercise 8.1: Program with two processes
 * Father reads from stdin and passes through pipe to son who displays on stdout
 */
int main(void) {
    int pipefd[2];
    pid_t pid;
    
    // Create pipe
    if (pipe(pipefd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }
    
    pid = fork();
    
    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    
    if (pid == 0) {
        // Son process
        close(pipefd[1]); // Close write end
        
        // Redirect stdout to stdout (default)
        // Read from pipe and display on stdout
        copier(pipefd[0], STDOUT_FILENO);
        
        close(pipefd[0]);
        exit(EXIT_SUCCESS);
    } else {
        // Father process
        close(pipefd[0]); // Close read end
        
        // Read from stdin and write to pipe
        copier(STDIN_FILENO, pipefd[1]);
        
        close(pipefd[1]);
        
        // Wait for son to finish
        wait(NULL);
    }
    
    return 0;
}