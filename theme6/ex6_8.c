#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s n m\n", argv[0]);
        fprintf(stderr, "  n: nombre de processus fils\n");
        fprintf(stderr, "  m: valeur maximale pour les nombres aléatoires\n");
        exit(1);
    }
    
    int n = atoi(argv[1]);
    int m = atoi(argv[2]);
    
    if (n <= 0 || m <= 0) {
        fprintf(stderr, "Les valeurs n et m doivent être positives\n");
        exit(1);
    }
    
    // Initialiser le générateur de nombres aléatoires
    srand(time(NULL));
    
    // Étape 1: Générer n nombres aléatoires entre 1 et m
    int aleatoires[n];
    int i;
    
    printf("Génération de %d nombres aléatoires entre 1 et %d:\n", n, m);
    for (i = 0; i < n; i++) {
        aleatoires[i] = rand() % m + 1;
        printf("a[%d] = %d\n", i + 1, aleatoires[i]);
    }
    
    // Étape 2: Créer n processus fils
    pid_t pids[n];
    
    printf("\nCréation des processus fils...\n");
    for (i = 0; i < n; i++) {
        pids[i] = fork();
        
        if (pids[i] < 0) {
            perror("Erreur lors du fork");
            exit(1);
        }
        
        if (pids[i] == 0) {
            // Processus fils
            printf("Fils %d (PID %d) dort pendant %d secondes\n", 
                   i + 1, getpid(), aleatoires[i]);
            
            sleep(aleatoires[i]);
            
            printf("Fils %d (PID %d) terminé, code de retour: %d\n", 
                   i + 1, getpid(), aleatoires[i]);
            
            exit(aleatoires[i]);
        }
    }
    
    // Étape 3: Attendre la terminaison des processus fils
    printf("\nAttente de la terminaison des processus fils...\n");
    
    for (i = 0; i < n; i++) {
        int status;
        pid_t pid_termine = wait(&status);
        
        if (WIFEXITED(status)) {
            int code_retour = WEXITSTATUS(status);
            printf("Père - Processus PID %d terminé avec code de retour: %d\n", 
                   pid_termine, code_retour);
        } else {
            printf("Père - Processus PID %d terminé anormalement\n", pid_termine);
        }
    }
    
    printf("Tous les processus fils sont terminés.\n");
    
    return 0;
}