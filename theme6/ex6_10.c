#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <dirent.h>

#define MAX_LINE 1024
#define MAX_ARGS 64

// Fonction pour chercher une commande dans le PATH
char* chercher_commande(char *cmd) {
    char *path = getenv("PATH");
    if (path == NULL) {
        return NULL;
    }
    
    char *path_copy = strdup(path);
    char *dir = strtok(path_copy, ":");
    
    while (dir != NULL) {
        char full_path[512];
        snprintf(full_path, sizeof(full_path), "%s/%s", dir, cmd);
        
        // Vérifier si le fichier existe et est exécutable
        if (access(full_path, X_OK) == 0) {
            free(path_copy);
            return strdup(full_path);
        }
        
        dir = strtok(NULL, ":");
    }
    
    free(path_copy);
    return NULL;
}

// Fonction pour parser une ligne en arguments
int parser_ligne(char *ligne, char **args) {
    int argc = 0;
    char *token;
    
    // Supprimer le caractère de fin de ligne
    ligne[strcspn(ligne, "\n")] = '\0';
    
    token = strtok(ligne, " \t");
    while (token != NULL && argc < MAX_ARGS - 1) {
        args[argc++] = token;
        token = strtok(NULL, " \t");
    }
    args[argc] = NULL;
    
    return argc;
}

int main(void) {
    char ligne[MAX_LINE];
    char *args[MAX_ARGS];
    
    printf("Shell simple - Tapez 'exit' pour quitter\n");
    
    while (1) {
        // Afficher le prompt
        printf("shell> ");
        fflush(stdout);
        
        // Lire une ligne
        if (fgets(ligne, sizeof(ligne), stdin) == NULL) {
            break; // EOF
        }
        
        // Parser la ligne
        int argc = parser_ligne(ligne, args);
        
        if (argc == 0) {
            continue; // Ligne vide
        }
        
        // Vérifier si c'est la commande exit
        if (strcmp(args[0], "exit") == 0) {
            printf("Au revoir!\n");
            break;
        }
        
        // Chercher la commande dans le PATH
        char *chemin_commande = chercher_commande(args[0]);
        if (chemin_commande == NULL) {
            fprintf(stderr, "Commande non trouvée: %s\n", args[0]);
            continue;
        }
        
        // Créer un processus fils pour exécuter la commande
        pid_t pid = fork();
        
        if (pid < 0) {
            perror("Erreur lors du fork");
            free(chemin_commande);
            continue;
        }
        
        if (pid == 0) {
            // Processus fils - exécuter la commande
            execv(chemin_commande, args);
            // Si on arrive ici, execv a échoué
            perror("Erreur lors de l'exécution");
            free(chemin_commande);
            exit(1);
        } else {
            // Processus père - attendre la terminaison du fils
            int status;
            waitpid(pid, &status, 0);
            free(chemin_commande);
        }
    }
    
    return 0;
}