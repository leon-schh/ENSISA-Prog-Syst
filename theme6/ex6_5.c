#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s repertoire\n", argv[0]);
        exit(1);
    }
    
    struct timeval debut, fin;
    
    // Afficher l'heure de début
    gettimeofday(&debut, NULL);
    printf("Heure de début: %ld.%06ld secondes\n", debut.tv_sec, debut.tv_usec);
    
    // Créer le processus fils pour exécuter ls -l
    pid_t pid = fork();
    
    if (pid < 0) {
        perror("Erreur lors du fork");
        exit(1);
    }
    
    if (pid == 0) {
        // Processus fils - exécuter ls -l
        execlp("ls", "ls", "-l", argv[1], NULL);
        perror("Erreur lors de l'exécution de ls");
        exit(1);
    } else {
        // Processus père - attendre la fin du fils
        int status;
        wait(&status);
        
        if (WIFEXITED(status)) {
            printf("ls s'est terminé avec le code: %d\n", WEXITSTATUS(status));
        }
        
        // Afficher l'heure de fin
        gettimeofday(&fin, NULL);
        printf("Heure de fin: %ld.%06ld secondes\n", fin.tv_sec, fin.tv_usec);
        
        // Calculer le temps écoulé
        long secondes = fin.tv_sec - debut.tv_sec;
        long microsecondes = fin.tv_usec - debut.tv_usec;
        if (microsecondes < 0) {
            microsecondes += 1000000;
            secondes--;
        }
        
        printf("Temps écoulé pendant l'appel de ls: %ld.%06ld secondes\n", 
               secondes, microsecondes);
    }
    
    return 0;
}