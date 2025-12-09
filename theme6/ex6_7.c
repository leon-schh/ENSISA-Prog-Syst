#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s n cmd arg1 arg2 ...\n", argv[0]);
        fprintf(stderr, "Exemple: %s 5 sh tester toto\n", argv[0]);
        exit(1);
    }
    
    int n = atoi(argv[1]);
    if (n <= 0) {
        fprintf(stderr, "Le nombre de processus doit être positif\n");
        exit(1);
    }
    
    // Préparer les arguments pour la commande
    // argv[2] = cmd, argv[3...] = args
    int nb_args = argc - 2;
    char *cmd = argv[2];
    
    pid_t pids[n];
    int i;
    
    // Créer n processus pour exécuter la commande en parallèle
    for (i = 0; i < n; i++) {
        pids[i] = fork();
        
        if (pids[i] < 0) {
            perror("Erreur lors du fork");
            exit(1);
        }
        
        if (pids[i] == 0) {
            // Processus fils - construire les arguments avec l'index
            char index_str[10];
            snprintf(index_str, sizeof(index_str), "%d", i);
            
            // Allouer un tableau d'arguments
            char **nouveaux_args = malloc((nb_args + 2) * sizeof(char *));
            if (nouveaux_args == NULL) {
                perror("Erreur malloc");
                exit(1);
            }
            
            // Copier les arguments originaux
            int j;
            for (j = 0; j < nb_args; j++) {
                nouveaux_args[j] = argv[j + 2];
            }
            
            // Ajouter l'index comme dernier argument
            nouveaux_args[nb_args] = index_str;
            nouveaux_args[nb_args + 1] = NULL;
            
            // Exécuter la commande
            execvp(cmd, nouveaux_args);
            
            // Si on arrive ici, execvp a échoué
            perror("Erreur lors de execvp");
            free(nouveaux_args);
            exit(1);
        }
    }
    
    // Attendre que tous les processus terminent
    int tous_reussis = 1;
    for (i = 0; i < n; i++) {
        int status;
        wait(&status);
        
        if (WIFEXITED(status)) {
            int code_retour = WEXITSTATUS(status);
            printf("Processus %d (PID %d) terminé avec code %d\n", 
                   i, pids[i], code_retour);
            if (code_retour != 0) {
                tous_reussis = 0;
            }
        } else {
            printf("Processus %d (PID %d) terminé anormalement\n", i, pids[i]);
            tous_reussis = 0;
        }
    }
    
    // Code de retour: 0 si tous ont réussi, 1 sinon
    exit(tous_reussis ? 0 : 1);
}