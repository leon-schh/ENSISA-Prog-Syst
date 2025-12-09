#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

void generer_processus(int niveau, int niveaux_max, int n) {
    if (niveau >= niveaux_max) {
        // Nous sommes au dernier niveau, arrêt de la récursion
        return;
    }
    
    int i;
    for (i = 0; i < n; i++) {
        pid_t pid = fork();
        
        if (pid < 0) {
            perror("Erreur lors du fork");
            exit(1);
        }
        
        if (pid == 0) {
            // Processus fils - continue la récursion
            generer_processus(niveau + 1, niveaux_max, n);
            exit(0);
        }
    }
}

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
    
    // Le programme principal génère n processus
    // chacun de ces processus génère n processus
    // et ainsi de suite jusqu'à n niveaux
    
    printf("Génération de %d niveaux avec %d processus par niveau\n", n, n);
    
    // Calcul du nombre total de processus
    int total = 1;
    int i;
    for (i = 0; i < n; i++) {
        total *= n;
    }
    printf("Nombre total de processus générés: %d^%d = %d\n", n, n, total);
    
    generer_processus(0, n, n);
    
    // Attendre que tous les processus terminent
    while (wait(NULL) > 0);
    
    return 0;
}