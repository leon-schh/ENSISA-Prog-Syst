#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
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
 * Exercise 8.2: Chain of n processes with pipes
 * n-1 processes in a chain, each passing data to the next
 */
int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <nombre_processus>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    
    int n = atoi(argv[1]);
    if (n < 2) {
        fprintf(stderr, "Le nombre de processus doit être >= 2\n");
        exit(EXIT_FAILURE);
    }
    
    // We'll create n-1 pipes for n processes
    int (*pipes)[2] = malloc((n-1) * sizeof(int[2]));
    if (!pipes) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    
    pid_t *pids = malloc(n * sizeof(pid_t));
    if (!pids) {
        perror("malloc");
        free(pipes);
        exit(EXIT_FAILURE);
    }
    
    // Create pipes
    for (int i = 0; i < n-1; i++) {
        if (pipe(pipes[i]) == -1) {
            perror("pipe");
            free(pipes);
            free(pids);
            exit(EXIT_FAILURE);
        }
    }
    
    // Create n-1 child processes
    for (int i = 0; i < n-1; i++) {
        pids[i] = fork();
        
        if (pids[i] == -1) {
            perror("fork");
            // Clean up already created processes
            for (int j = 0; j < i; j++) {
                kill(pids[j], SIGTERM);
                waitpid(pids[j], NULL, 0);
            }
            free(pipes);
            free(pids);
            exit(EXIT_FAILURE);
        }
        
        if (pids[i] == 0) {
            // Child process i
            // Close unused pipe ends
            for (int j = 0; j < n-1; j++) {
                if (j != i-1) close(pipes[j][0]); // Not read end of previous pipe
                if (j != i) close(pipes[j][1]);   // Not write end of current pipe
            }
            
            if (i == 0) {
                // First process: reads from stdin, writes to first pipe
                close(pipes[0][0]); // Close read end
                copier(STDIN_FILENO, pipes[0][1]);
                close(pipes[0][1]);
            } else {
                // Middle processes: reads from previous pipe, writes to next pipe
                close(pipes[i][1]); // Close write end of current pipe
                copier(pipes[i-1][0], pipes[i][0] == -1 ? STDOUT_FILENO : pipes[i][1]);
                
                if (pipes[i][0] != -1) close(pipes[i][0]);
                if (pipes[i][1] != -1) close(pipes[i][1]);
                if (pipes[i-1][0] != -1) close(pipes[i-1][0]);
            }
            
            free(pipes);
            free(pids);
            exit(EXIT_SUCCESS);
        }
    }
    
    // Father process
    // Close all pipe ends except the last read end
    for (int i = 0; i < n-2; i++) {
        close(pipes[i][0]);
        close(pipes[i][1]);
    }
    close(pipes[n-2][1]); // Close write end of last pipe
    
    // Read from last pipe and write to stdout
    copier(pipes[n-2][0], STDOUT_FILENO);
    close(pipes[n-2][0]);
    
    // Wait for all children
    for (int i = 0; i < n-1; i++) {
        wait(NULL);
    }
    
    free(pipes);
    free(pids);
    
    return 0;
}