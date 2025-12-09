#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <signal.h>

#define MAX_ARGS 64
#define BUFFER_SIZE 1024

/**
 * Parse command line and detect pipes and background execution
 */
int parse_command(char *line, char ***commands, int *background, int *num_commands) {
    char *commands_copy = strdup(line);
    if (!commands_copy) return -1;
    
    *num_commands = 0;
    *background = 0;
    
    // Check for background execution
    char *bg_marker = strstr(commands_copy, "&");
    if (bg_marker) {
        *background = 1;
        *bg_marker = '\0'; // Remove & from command
    }
    
    // Split by pipes
    char *token;
    char *saveptr;
    
    // First command
    token = strtok_r(commands_copy, "|", &saveptr);
    while (token != NULL && *num_commands < MAX_ARGS) {
        (*commands)[*num_commands] = strdup(token);
        if (!(*commands)[*num_commands]) {
            // Clean up
            for (int i = 0; i < *num_commands; i++) {
                free((*commands)[i]);
            }
            free(commands_copy);
            return -1;
        }
        (*num_commands)++;
        token = strtok_r(NULL, "|", &saveptr);
    }
    
    free(commands_copy);
    return 0;
}

/**
 * Parse arguments for a single command
 */
int parse_args(char *cmd, char **args) {
    int argc = 0;
    char *token;
    char *saveptr;
    char *cmd_copy = strdup(cmd);
    if (!cmd_copy) return -1;
    
    token = strtok_r(cmd_copy, " \t\n", &saveptr);
    while (token != NULL && argc < MAX_ARGS - 1) {
        args[argc++] = strdup(token);
        token = strtok_r(NULL, " \t\n", &saveptr);
    }
    args[argc] = NULL;
    
    free(cmd_copy);
    return argc;
}

/**
 * Execute a single command with redirections
 */
void execute_command(char **args, int background) {
    pid_t pid = fork();
    
    if (pid == -1) {
        perror("fork");
        return;
    }
    
    if (pid == 0) {
        // Child process
        execvp(args[0], args);
        perror("execvp");
        exit(EXIT_FAILURE);
    } else {
        // Parent process
        if (!background) {
            int status;
            waitpid(pid, &status, 0);
        }
        // For background processes, we don't wait
    }
}

/**
 * Execute piped commands
 */
void execute_piped_commands(char **commands, int num_commands, int background) {
    if (num_commands == 1) {
        // Single command, no pipes
        char *args[MAX_ARGS];
        parse_args(commands[0], args);
        execute_command(args, background);
        
        // Clean up args
        for (int i = 0; args[i] != NULL; i++) {
            free(args[i]);
        }
    } else {
        // Multiple commands with pipes
        int (*pipes)[2] = malloc((num_commands - 1) * sizeof(int[2]));
        if (!pipes) {
            perror("malloc");
            return;
        }
        
        // Create pipes
        for (int i = 0; i < num_commands - 1; i++) {
            if (pipe(pipes[i]) == -1) {
                perror("pipe");
                free(pipes);
                return;
            }
        }
        
        // Execute commands
        for (int i = 0; i < num_commands; i++) {
            pid_t pid = fork();
            
            if (pid == -1) {
                perror("fork");
                free(pipes);
                return;
            }
            
            if (pid == 0) {
                // Child process
                char *args[MAX_ARGS];
                parse_args(commands[i], args);
                
                // Setup redirections
                if (i > 0) {
                    // Not first command: redirect stdin from previous pipe
                    dup2(pipes[i-1][0], STDIN_FILENO);
                }
                if (i < num_commands - 1) {
                    // Not last command: redirect stdout to next pipe
                    dup2(pipes[i][1], STDOUT_FILENO);
                }
                
                // Close all pipe ends
                for (int j = 0; j < num_commands - 1; j++) {
                    close(pipes[j][0]);
                    close(pipes[j][1]);
                }
                
                // Execute command
                execvp(args[0], args);
                perror("execvp");
                exit(EXIT_FAILURE);
            }
        }
        
        // Close all pipe ends in parent
        for (int i = 0; i < num_commands - 1; i++) {
            close(pipes[i][0]);
            close(pipes[i][1]);
        }
        
        // Wait for all children if not background
        if (!background) {
            for (int i = 0; i < num_commands; i++) {
                wait(NULL);
            }
        }
        
        free(pipes);
    }
}

/**
 * Exercise 8.5: Shell with pipes and background execution
 */
int main(void) {
    char line[BUFFER_SIZE];
    char *commands[MAX_ARGS];
    
    printf("Shell avec support des pipes et de l'exécution en arrière-plan\n");
    printf("Tapez 'quit' pour quitter\n");
    
    while (1) {
        printf("> ");
        fflush(stdout);
        
        if (fgets(line, sizeof(line), stdin) == NULL) {
            break;
        }
        
        // Remove newline
        line[strcspn(line, "\n")] = '\0';
        
        // Check for quit command
        if (strcmp(line, "quit") == 0) {
            break;
        }
        
        // Skip empty lines
        if (strlen(line) == 0) {
            continue;
        }
        
        int background, num_commands;
        if (parse_command(line, &commands, &background, &num_commands) == -1) {
            fprintf(stderr, "Erreur lors du parsing de la commande\n");
            continue;
        }
        
        execute_piped_commands(commands, num_commands, background);
        
        // Clean up commands
        for (int i = 0; i < num_commands; i++) {
            free(commands[i]);
        }
    }
    
    return 0;
}