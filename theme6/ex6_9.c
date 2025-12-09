#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <string.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s utilisateur\n", argv[0]);
        exit(1);
    }
    
    const char *utilisateur = argv[1];
    int fd_toto, fd_grep, fd_devnull;
    pid_t pid_ps, pid_grep;
    int tube[2];
    
    // Créer un tube pour la communication ps -> grep
    if (pipe(tube) < 0) {
        perror("Erreur lors de la création du tube");
        exit(1);
    }
    
    // Étape 1: Exécuter ps eaux et rediriger vers toto
    pid_ps = fork();
    if (pid_ps < 0) {
        perror("Erreur lors du fork pour ps");
        exit(1);
    }
    
    if (pid_ps == 0) {
        // Processus fils - exécuter ps eaux > toto
        fd_toto = open("toto", O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (fd_toto < 0) {
            perror("Erreur lors de l'ouverture de toto");
            exit(1);
        }
        
        // Rediriger stdout vers toto
        dup2(fd_toto, STDOUT_FILENO);
        close(fd_toto);
        
        execlp("ps", "ps", "eaux", NULL);
        perror("Erreur lors de l'exécution de ps");
        exit(1);
    }
    
    // Étape 2: Exécuter grep "^$1 " < toto > /dev/null
    pid_grep = fork();
    if (pid_grep < 0) {
        perror("Erreur lors du fork pour grep");
        exit(1);
    }
    
    if (pid_grep == 0) {
        // Processus fils - exécuter grep
        // Ouvrir toto en lecture
        fd_toto = open("toto", O_RDONLY);
        if (fd_toto < 0) {
            perror("Erreur lors de l'ouverture de toto");
            exit(1);
        }
        
        // Ouvrir /dev/null en écriture
        fd_devnull = open("/dev/null", O_WRONLY);
        if (fd_devnull < 0) {
            perror("Erreur lors de l'ouverture de /dev/null");
            exit(1);
        }
        
        // Rediriger stdin depuis toto
        dup2(fd_toto, STDIN_FILENO);
        // Rediriger stdout vers /dev/null
        dup2(fd_devnull, STDOUT_FILENO);
        
        close(fd_toto);
        close(fd_devnull);
        
        // Construire le motif de recherche
        char motif[256];
        snprintf(motif, sizeof(motif), "^%s ", utilisateur);
        
        execlp("grep", "grep", motif, NULL);
        perror("Erreur lors de l'exécution de grep");
        exit(1);
    }
    
    // Père: attendre la terminaison des processus
    int status_ps, status_grep;
    waitpid(pid_ps, &status_ps, 0);
    waitpid(pid_grep, &status_grep, 0);
    
    // Vérifier si grep a trouvé quelque chose (code de retour 0)
    if (WIFEXITED(status_grep) && WEXITSTATUS(status_grep) == 0) {
        // L'utilisateur est connecté
        char message[256];
        snprintf(message, sizeof(message), "%s est connecté\n", utilisateur);
        write(STDOUT_FILENO, message, strlen(message));
    }
    
    // Nettoyer: supprimer le fichier temporaire
    unlink("toto");
    
    return 0;
}