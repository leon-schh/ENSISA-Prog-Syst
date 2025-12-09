#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

#define MAX_LINE 1024
#define MAX_ARGS 64

// Structure pour stocker les informations de redirection et d'exécution
typedef struct {
    char *fichier_entree;
    char *fichier_sortie;
    int mode_sortie; // 0: écraser, 1: ajouter
    int background;  // 0: premier plan, 1: arrière-plan
} exec_info_t;

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
        
        if (access(full_path, X_OK) == 0) {
            free(path_copy);
            return strdup(full_path);
        }
        
        dir = strtok(NULL, ":");
    }
    
    free(path_copy);
    return NULL;
}

// Fonction pour parser une ligne avec gestion des redirections et de l'arrière-plan
int parser_ligne(char *ligne, char **args, exec_info_t *info) {
    int argc = 0;
    char *token;
    
    // Initialiser les informations d'exécution
    info->fichier_entree = NULL;
    info->fichier_sortie = NULL;
    info->mode_sortie = 0;
    info->background = 0;
    
    // Supprimer le caractère de fin de ligne
    ligne[strcspn(ligne, "\n")] = '\0';
    
    token = strtok(ligne, " \t");
    while (token != NULL && argc < MAX_ARGS - 1) {
        if (strcmp(token, "<") == 0) {
            token = strtok(NULL, " \t");
            if (token != NULL) {
                info->fichier_entree = strdup(token);
            }
        } else if (strcmp(token, ">") == 0) {
            token = strtok(NULL, " \t");
            if (token != NULL) {
                info->fichier_sortie = strdup(token);
                info->mode_sortie = 0;
            }
        } else if (strcmp(token, ">>") == 0) {
            token = strtok(NULL, " \t");
            if (token != NULL) {
                info->fichier_sortie = strdup(token);
                info->mode_sortie = 1;
            }
        } else if (strcmp(token, "&") == 0) {
            info->background = 1;
        } else {
            args[argc++] = token;
        }
        
        token = strtok(NULL, " \t");
    }
    args[argc] = NULL;
    
    return argc;
}

// Fonction pour nettoyer les processus fils terminés en arrière-plan
void nettoyer_processus() {
    while (waitpid(-1, NULL, WNOHANG) > 0) {
        // Un processus fils en arrière-plan s'est terminé
    }
}

int main(void) {
    char ligne[MAX_LINE];
    char *args[MAX_ARGS];
    exec_info_t info;
    
    printf("Shell complet - Tapez 'exit' pour quitter\n");
    printf("Syntaxe: commande [args] [< fichier] [> fichier] [>> fichier] [&]\n");
    printf("L'opérateur '&' lance la commande en arrière-plan\n");
    
    while (1) {
        // Nettoyer les processus terminés avant d'afficher le prompt
        nettoyer_processus();
        
        printf("shell> ");
        fflush(stdout);
        
        if (fgets(ligne, sizeof(ligne), stdin) == NULL) {
            break;
        }
        
        int argc = parser_ligne(ligne, args, &info);
        
        if (argc == 0) {
            continue;
        }
        
        if (strcmp(args[0], "exit") == 0) {
            printf("Au revoir!\n");
            // Attendre tous les processus en arrière-plan
            while (waitpid(-1, NULL, WNOHANG) > 0);
            break;
        }
        
        char *chemin_commande = chercher_commande(args[0]);
        if (chemin_commande == NULL) {
            fprintf(stderr, "Commande non trouvée: %s\n", args[0]);
            continue;
        }
        
        pid_t pid = fork();
        
        if (pid < 0) {
            perror("Erreur lors du fork");
            free(chemin_commande);
            continue;
        }
        
        if (pid == 0) {
            // Processus fils - gérer les redirections
            
            // Redirection d'entrée
            if (info.fichier_entree) {
                int fd = open(info.fichier_entree, O_RDONLY);
                if (fd < 0) {
                    perror("Erreur lors de l'ouverture du fichier d'entrée");
                    exit(1);
                }
                dup2(fd, STDIN_FILENO);
                close(fd);
            }
            
            // Redirection de sortie
            if (info.fichier_sortie) {
                int flags = O_WRONLY | O_CREAT;
                if (info.mode_sortie == 0) {
                    flags |= O_TRUNC;
                } else {
                    flags |= O_APPEND;
                }
                
                int fd = open(info.fichier_sortie, flags, 0644);
                if (fd < 0) {
                    perror("Erreur lors de l'ouverture du fichier de sortie");
                    exit(1);
                }
                dup2(fd, STDOUT_FILENO);
                close(fd);
            }
            
            // Exécuter la commande
            execv(chemin_commande, args);
            perror("Erreur lors de l'exécution");
            free(chemin_commande);
            if (info.fichier_entree) free(info.fichier_entree);
            if (info.fichier_sortie) free(info.fichier_sortie);
            exit(1);
        } else {
            // Processus père
            if (info.background) {
                printf("[%d] %d\n", pid, pid);
            } else {
                int status;
                waitpid(pid, &status, 0);
            }
            
            free(chemin_commande);
            if (info.fichier_entree) free(info.fichier_entree);
            if (info.fichier_sortie) free(info.fichier_sortie);
        }
    }
    
    return 0;
}