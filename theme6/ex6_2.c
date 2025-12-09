#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s n\n", argv[0]);
        exit(1);
    }
    
    int n = atoi(argv[1]);
    if (n <= 0) {
        fprintf(stderr, "Le nombre de processus doit être positif\n");
        exit(1);
    }
    
    pid_t pids[n];
    int i;
    
    // Créer n processus fils
    for (i = 0; i < n; i++) {
        pids[i] = fork();
        
        if (pids[i] < 0) {
            perror("Erreur lors du fork");
            exit(1);
        }
        
        if (pids[i] == 0) {
            // Processus fils
            printf("Fils %d - PID: %d\n", i + 1, getpid());
            exit(i + 1); // Code de retour = numéro du fils
        }
    }
    
    // Attendre la terminaison de tous les processus fils
    for (i = 0; i < n; i++) {
        int status;
        pid_t pid_termine = wait(&status);
        
        if (WIFEXITED(status)) {
            printf("Père - Processus PID %d terminé avec code %d\n", 
                   pid_termine, WEXITSTATUS(status));
        }
    }
    
    return 0;
}