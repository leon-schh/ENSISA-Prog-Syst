#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <errno.h>
#include <string.h>

#define BUFFER_SIZE 1024

/**
 * Signal handler to display pipe capacity when writer blocks
 */
void signal_handler(int sig) {
    extern ssize_t total_written;
    printf("Taille du tube: %zd octets\n", total_written);
    exit(0);
}

/**
 * Exercise 8.3: Determine pipe capacity
 * Write to a pipe with no reader to find when it blocks
 */
int main(void) {
    int pipefd[2];
    pid_t pid;
    ssize_t total_written = 0;
    char buffer[BUFFER_SIZE];
    
    // Make total_written accessible to signal handler
    extern ssize_t total_written;
    
    // Set up signal handler
    signal(SIGUSR1, signal_handler);
    
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
        // Son process - reader that never reads
        close(pipefd[1]); // Close write end
        
        // Just sleep forever
        while (1) {
            sleep(1);
        }
        
        exit(EXIT_SUCCESS);
    } else {
        // Father process - writer that fills the pipe
        close(pipefd[0]); // Close read end
        
        printf("Processus père (PID: %d) commence à écrire dans le tube...\n", getpid());
        printf("Processus fils (PID: %d) ouvert en lecture mais ne lit jamais.\n", pid);
        printf("Signal envoyé par: kill -USR1 %d\n", getpid());
        
        // Fill the pipe until it blocks
        while (1) {
            ssize_t written = write(pipefd[1], buffer, BUFFER_SIZE);
            
            if (written == -1) {
                if (errno == EPIPE) {
                    printf("Broken pipe detected\n");
                    break;
                } else if (errno == EAGAIN || errno == EWOULDBLOCK) {
                    printf("Tube plein! Le père se bloque en attente.\n");
                    printf("Pour voir la capacité, envoyez le signal: kill -USR1 %d\n", getpid());
                    
                    // Wait for signal
                    while (1) {
                        pause(); // Wait for signal
                    }
                    break;
                } else {
                    perror("write");
                    break;
                }
            }
            
            total_written += written;
            
            // Print progress every 10KB
            if (total_written % (10 * 1024) == 0) {
                printf("Écrits: %zd octets...\n", total_written);
            }
        }
        
        printf("Capacité totale du tube: %zd octets\n", total_written);
        
        close(pipefd[1]);
        kill(pid, SIGTERM); // Terminate son
        wait(NULL);
    }
    
    return 0;
}