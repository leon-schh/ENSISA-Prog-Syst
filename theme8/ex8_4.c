#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

#define BUFFER_SIZE 4096

/**
 * Exercise 8.4: Create equivalent of shell command
 * ps eaux | grep "<nom>" | wc -l
 * The name will be given by the user as argument, or default to USER environment variable
 */
int main(int argc, char *argv[]) {
    int pipe1[2], pipe2[2];
    pid_t pid1, pid2, pid3;
    char *username;
    
    // Get username from argument or environment
    if (argc > 1) {
        username = argv[1];
    } else {
        username = getenv("USER");
        if (!username) {
            fprintf(stderr, "Aucun nom d'utilisateur fourni et variable USER non définie\n");
            exit(EXIT_FAILURE);
        }
    }
    
    printf("Recherche des processus de l'utilisateur: %s\n", username);
    
    // Create pipes
    if (pipe(pipe1) == -1) {
        perror("pipe1");
        exit(EXIT_FAILURE);
    }
    
    if (pipe(pipe2) == -1) {
        perror("pipe2");
        exit(EXIT_FAILURE);
    }
    
    // First child: ps eaux
    pid1 = fork();
    if (pid1 == -1) {
        perror("fork1");
        exit(EXIT_FAILURE);
    }
    
    if (pid1 == 0) {
        // First child: ps eaux
        close(pipe1[0]); // Close read end of first pipe
        close(pipe2[0]); // Close both ends of second pipe
        close(pipe2[1]);
        
        // Redirect stdout to write end of first pipe
        dup2(pipe1[1], STDOUT_FILENO);
        close(pipe1[1]);
        
        // Execute ps command
        execlp("ps", "ps", "eaux", NULL);
        perror("execlp ps");
        exit(EXIT_FAILURE);
    }
    
    // Second child: grep
    pid2 = fork();
    if (pid2 == -1) {
        perror("fork2");
        exit(EXIT_FAILURE);
    }
    
    if (pid2 == 0) {
        // Second child: grep
        close(pipe1[1]); // Close write end of first pipe
        close(pipe2[0]); // Close read end of second pipe
        
        // Redirect stdin to read end of first pipe
        dup2(pipe1[0], STDIN_FILENO);
        close(pipe1[0]);
        
        // Redirect stdout to write end of second pipe
        dup2(pipe2[1], STDOUT_FILENO);
        close(pipe2[1]);
        
        // Build grep command with pattern
        char pattern[256];
        snprintf(pattern, sizeof(pattern), "^%s ", username);
        
        // Execute grep command
        execlp("grep", "grep", pattern, NULL);
        perror("execlp grep");
        exit(EXIT_FAILURE);
    }
    
    // Third child: wc -l
    pid3 = fork();
    if (pid3 == -1) {
        perror("fork3");
        exit(EXIT_FAILURE);
    }
    
    if (pid3 == 0) {
        // Third child: wc -l
        close(pipe1[0]); // Close both ends of first pipe
        close(pipe1[1]);
        close(pipe2[1]); // Close write end of second pipe
        
        // Redirect stdin to read end of second pipe
        dup2(pipe2[0], STDIN_FILENO);
        close(pipe2[0]);
        
        // Execute wc command
        execlp("wc", "wc", "-l", NULL);
        perror("execlp wc");
        exit(EXIT_FAILURE);
    }
    
    // Father process
    // Close all pipe ends
    close(pipe1[0]);
    close(pipe1[1]);
    close(pipe2[0]);
    close(pipe2[1]);
    
    // Wait for all children
    int status;
    for (int i = 0; i < 3; i++) {
        wait(&status);
        if (WIFEXITED(status) && WEXITSTATUS(status) != 0) {
            fprintf(stderr, "Un des processus fils s'est terminé avec une erreur\n");
        }
    }
    
    printf("Recherche terminée\n");
    
    return 0;
}