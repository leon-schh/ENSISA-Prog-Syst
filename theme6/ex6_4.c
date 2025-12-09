#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <string.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s fichier\n", argv[0]);
        exit(1);
    }
    
    // Ouvrir le fichier en lecture
    int fd = open(argv[1], O_RDONLY);
    if (fd < 0) {
        perror("Erreur lors de l'ouverture du fichier");
        exit(1);
    }
    
    // Créer le processus fils
    pid_t pid = fork();
    
    if (pid < 0) {
        perror("Erreur lors du fork");
        exit(1);
    }
    
    char buffer[1];
    int total_lu = 0;
    
    if (pid == 0) {
        // Processus fils
        printf("Fils - PID: %d, commence la lecture\n", getpid());
        
        while (read(fd, buffer, 1) > 0) {
            printf("Fils a lu: %c", buffer[0]);
            total_lu++;
        }
        
        printf("\nFils - Total lu: %d caractères\n", total_lu);
        close(fd);
        exit(0);
    } else {
        // Processus père
        printf("Père - PID: %d, PID du fils: %d, commence la lecture\n", getpid(), pid);
        
        // Repositionner le curseur au début du fichier
        lseek(fd, 0, SEEK_SET);
        
        while (read(fd, buffer, 1) > 0) {
            printf("Père a lu: %c", buffer[0]);
            total_lu++;
        }
        
        printf("\nPère - Total lu: %d caractères\n", total_lu);
        
        // Attendre la terminaison du fils
        int status;
        wait(&status);
        
        printf("Père - Fils terminé\n");
        close(fd);
    }
    
    return 0;
}