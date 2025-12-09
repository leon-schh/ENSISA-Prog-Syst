#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

#define MAX_LINE 1024
#define MAX_ARGS 64

// Structure pour stocker les informations de redirection
typedef struct {
    char *fichier_entree;
    char *fichier_sortie;
    int mode_sortie; // 0: écraser, 1: ajouter
} redirections_t;

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

// Fonction pour parser une ligne avec gestion des redirections
int parser_ligne(char *ligne, char **args, redirections_t *redir) {
    int argc = 0;
    char *token;
    
    // Initialiser les redirections
    redir->fichier_entree = NULL;
    redir->fichier_sortie = NULL;
    redir->mode_sortie = 0;
    
    // Supprimer le caractère de fin de ligne
    ligne[strcspn(ligne, "\n")] = '\0';
    
    token = strtok(ligne, " \t");
    while (token != NULL && argc < MAX_ARGS - 1) {
        if (strcmp(token, "<") == 0) {
            // Redirection d'entrée
            token = strtok(NULL, " \t");
            if (token != NULL) {
                redir->fichier_entree = strdup(token);
            }
        } else if (strcmp(token, ">") == 0) {
            // Redirection de sortie (écrasement)
            token = strtok(NULL, " \t");
            if (token != NULL) {
                redir->fichier_sortie = strdup(token);
                redir->mode_sortie = 0;
            }
        } else if (strcmp(token, ">>") == 0) {
            // Redirection de sortie (ajout)
            token = strtok(NULL, " \t");
            if (token != NULL) {
                redir->fichier_sortie = strdup(token);
                redir->mode_sortie = 1;
            }
        } else {
            args[argc++] = token;
        }
        
        token = strtok(NULL, " \t");
    }
    args[argc] = NULL;
    
    return argc;
}

int main(void) {
    char ligne[MAX_LINE];
    char *args[MAX_ARGS];
    redirections_t redir;
    
    printf("Shell simple avec redirections - Tapez 'exit' pour quitter\n");
    printf("Syntaxe: commande [args] [< fichier] [> fichier] [>> fichier]\n");
    
    while (1) {
        printf("shell> ");
        fflush(stdout);
        
        if (fgets(ligne, sizeof(ligne), stdin) == NULL) {
            break;
        }
        
        int argc = parser_ligne(ligne, args, &redir);
        
        if (argc == 0) {
            continue;
        }
        
        if (strcmp(args[0], "exit") == 0) {
            printf("Au revoir!\n");
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
            if (redir.fichier_entree) {
                int fd = open(redir.fichier_entree, O_RDONLY);
                if (fd < 0) {
                    perror("Erreur lors de l'ouverture du fichier d'entrée");
                    exit(1);
                }
                dup2(fd, STDIN_FILENO);
                close(fd);
            }
            
            // Redirection de sortie
            if (redir.fichier_sortie) {
                int flags = O_WRONLY | O_CREAT;
                if (redir.mode_sortie == 0) {
                    flags |= O_TRUNC; // Écraser
                } else {
                    flags |= O_APPEND; // Ajouter
                }
                
                int fd = open(redir.fichier_sortie, flags, 0644);
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
            if (redir.fichier_entree) free(redir.fichier_entree);
            if (redir.fichier_sortie) free(redir.fichier_sortie);
            exit(1);
        } else {
            // Processus père
            int status;
            waitpid(pid, &status, 0);
            
            free(chemin_commande);
            if (redir.fichier_entree) free(redir.fichier_entree);
            if (redir.fichier_sortie) free(redir.fichier_sortie);
        }
    }
    
    return 0;
}