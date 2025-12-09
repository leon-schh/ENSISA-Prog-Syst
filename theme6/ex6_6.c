#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/times.h>
#include <sys/wait.h>
#include <fcntl.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s repertoire\n", argv[0]);
        exit(1);
    }
    
    struct tms temps_debut, temps_fin;
    clock_t horloge_debut, horloge_fin;
    
    // Enregistrer le temps de début
    horloge_debut = times(&temps_debut);
    
    // Créer le processus fils pour exécuter ls -R
    pid_t pid = fork();
    
    if (pid < 0) {
        perror("Erreur lors du fork");
        exit(1);
    }
    
    if (pid == 0) {
        // Processus fils - rediriger la sortie vers /dev/null
        int fd = open("/dev/null", O_WRONLY);
        if (fd < 0) {
            perror("Erreur lors de l'ouverture de /dev/null");
            exit(1);
        }
        
        // Rediriger stdout et stderr vers /dev/null
        dup2(fd, STDOUT_FILENO);
        dup2(fd, STDERR_FILENO);
        close(fd);
        
        // Exécuter ls -R
        execlp("ls", "ls", "-R", argv[1], NULL);
        perror("Erreur lors de l'exécution de ls");
        exit(1);
    } else {
        // Processus père - attendre la fin du fils
        int status;
        wait(&status);
        
        if (WIFEXITED(status)) {
            printf("ls -R s'est terminé avec le code: %d\n", WEXITSTATUS(status));
        }
        
        // Enregistrer le temps de fin
        horloge_fin = times(&temps_fin);
        
        // Calculer les temps CPU
        double temps_utilisateur = (temps_fin.tms_utime - temps_debut.tms_utime) / (double)sysconf(_SC_CLK_TCK);
        double temps_systeme = (temps_fin.tms_stime - temps_debut.tms_stime) / (double)sysconf(_SC_CLK_TCK);
        double temps_total = temps_utilisateur + temps_systeme;
        
        printf("Temps CPU utilisateur: %.2f secondes\n", temps_utilisateur);
        printf("Temps CPU système: %.2f secondes\n", temps_systeme);
        printf("Somme des temps CPU: %.2f secondes\n", temps_total);
    }
    
    return 0;
}