/*
 * Exercice 7.5 : Fonction getcwd
 * 
 * Écrivez une nouvelle version de la fonction getcwd. Pour cela,
 * on cherchera le numéro d'inode du répertoire courant. Puis, on
 * cherchera dans le répertoire parent le nom correspondant à l'inode
 * du répertoire courant.
 * 
 * En répétant cette opération jusqu'au répertoire d'inode numéro 2
 * (la racine du système de fichiers), on peut reconstituer le nom
 * du répertoire courant.
 * 
 * On notera que cette méthode ne tient compte que du système de
 * fichiers courant. On n'essayera pas de s'affranchir de cette
 * limitation.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>

#define MAX_PATH_LENGTH 4096

/* Notre implémentation de getcwd */
char *mon_getcwd(char *buf, size_t size)
{
    struct stat cwd_stat, parent_stat, entry_stat;
    DIR *dir;
    struct dirent *entry;
    char *path_components[1024]; /* Composants du chemin */
    int component_count = 0;
    char *result;
    int i;
    
    /* Vérifier les paramètres */
    if (buf == NULL) {
        /* Allouer un buffer si nécessaire */
        buf = malloc(size > 0 ? size : MAX_PATH_LENGTH);
        if (buf == NULL) {
            return NULL;
        }
    } else if (size == 0) {
        errno = EINVAL;
        return NULL;
    }
    
    /* Obtenir les informations du répertoire courant */
    if (stat(".", &cwd_stat) < 0) {
        return NULL;
    }
    
    /* Vérifier que c'est bien un répertoire */
    if (!S_ISDIR(cwd_stat.st_mode)) {
        errno = ENOTDIR;
        return NULL;
    }
    
    /* Si on est à la racine (inode 2), retourner "/" */
    if (cwd_stat.st_ino == 2) {
        if (size < 2) {
            errno = ERANGE;
            return NULL;
        }
        strcpy(buf, "/");
        return buf;
    }
    
    /* Remonter l'arbre des répertoires */
    while (cwd_stat.st_ino != 2) {
        /* Ouvrir le répertoire parent */
        dir = opendir("..");
        if (dir == NULL) {
            return NULL;
        }
        
        /* Obtenir les informations du répertoire parent */
        if (stat("..", &parent_stat) < 0) {
            closedir(dir);
            return NULL;
        }
        
        /* Chercher dans le répertoire parent l'entrée qui correspond
         * à notre inode courant */
        while ((entry = readdir(dir)) != NULL) {
            /* Ignorer "." et ".." */
            if (strcmp(entry->d_name, ".") == 0 || 
                strcmp(entry->d_name, "..") == 0) {
                continue;
            }
            
            /* Construire le chemin de l'entrée */
            char entry_path[MAX_PATH_LENGTH];
            snprintf(entry_path, sizeof(entry_path), "../%s", entry->d_name);
            
            /* Obtenir les informations sur cette entrée */
            if (stat(entry_path, &entry_stat) < 0) {
                continue;
            }
            
            /* Vérifier si c'est le même inode que notre répertoire courant */
            if (entry_stat.st_ino == cwd_stat.st_ino &&
                entry_stat.st_dev == cwd_stat.st_dev) {
                /* Trouvé ! Sauvegarder ce composant */
                if (component_count >= 1024) {
                    closedir(dir);
                    errno = ENAMETOOLONG;
                    return NULL;
                }
                
                path_components[component_count] = strdup(entry->d_name);
                if (path_components[component_count] == NULL) {
                    closedir(dir);
                    return NULL;
                }
                component_count++;
                break;
            }
        }
        
        closedir(dir);
        
        /* Si on n'a pas trouvé l'entrée, il y a un problème */
        if (entry == NULL) {
            /* Nettoyer la mémoire allouée */
            for (i = 0; i < component_count; i++) {
                free(path_components[i]);
            }
            errno = ENOENT;
            return NULL;
        }
        
        /* Remonter dans le répertoire parent */
        if (chdir("..") < 0) {
            /* Nettoyer la mémoire */
            for (i = 0; i < component_count; i++) {
                free(path_components[i]);
            }
            return NULL;
        }
        
        /* Préparer pour la prochaine itération */
        cwd_stat = parent_stat;
    }
    
    /* Construire le chemin final */
    if (component_count == 0) {
        /* On est à la racine */
        if (size < 2) {
            errno = ERANGE;
            return NULL;
        }
        strcpy(buf, "/");
    } else {
        /* Construire le chemin en inversant les composants */
        char temp_buf[MAX_PATH_LENGTH];
        temp_buf[0] = '\0';
        
        for (i = component_count - 1; i >= 0; i--) {
            strcat(temp_buf, "/");
            strcat(temp_buf, path_components[i]);
        }
        
        /* Copier dans le buffer de destination */
        if (strlen(temp_buf) >= size) {
            /* Nettoyer la mémoire */
            for (i = 0; i < component_count; i++) {
                free(path_components[i]);
            }
            errno = ERANGE;
            return NULL;
        }
        
        strcpy(buf, temp_buf);
    }
    
    /* Nettoyer la mémoire */
    for (i = 0; i < component_count; i++) {
        free(path_components[i]);
    }
    
    return buf;
}

/* Fonction pour tester mon_getcwd */
void tester_getcwd(void)
{
    char buf1[MAX_PATH_LENGTH], buf2[MAX_PATH_LENGTH];
    char *result1, *result2;
    
    printf("=== Test de mon_getcwd ===\n\n");
    
    /* Test avec notre implémentation */
    result1 = mon_getcwd(buf1, sizeof(buf1));
    if (result1 != NULL) {
        printf("mon_getcwd() = '%s'\n", result1);
    } else {
        printf("mon_getcwd() = NULL (erreur: %s)\n", strerror(errno));
    }
    
    /* Comparer avec la vraie fonction */
    result2 = getcwd(buf2, sizeof(buf2));
    if (result2 != NULL) {
        printf("getcwd()     = '%s'\n", result2);
    } else {
        printf("getcwd()     = NULL (erreur: %s)\n", strerror(errno));
    }
    
    /* Vérifier si les résultats sont identiques */
    if (result1 != NULL && result2 != NULL) {
        if (strcmp(result1, result2) == 0) {
            printf("✓ Les résultats sont identiques\n");
        } else {
            printf("✗ Les résultats different!\n");
            printf("  mon_getcwd: '%s'\n", result1);
            printf("  getcwd:     '%s'\n", result2);
        }
    }
    printf("\n");
}

/* Test dans différents répertoires */
void tester_dans_differents_repertoires(void)
{
    char buf[MAX_PATH_LENGTH];
    
    printf("=== Test dans différents répertoires ===\n\n");
    
    /* Test dans /tmp */
    if (chdir("/tmp") == 0) {
        printf("Répertoire courant: /tmp\n");
        if (mon_getcwd(buf, sizeof(buf)) != NULL) {
            printf("mon_getcwd() = '%s'\n", buf);
        } else {
            printf("mon_getcwd() = NULL (erreur: %s)\n", strerror(errno));
        }
        printf("\n");
    }
    
    /* Test dans /usr */
    if (chdir("/usr") == 0) {
        printf("Répertoire courant: /usr\n");
        if (mon_getcwd(buf, sizeof(buf)) != NULL) {
            printf("mon_getcwd() = '%s'\n", buf);
        } else {
            printf("mon_getcwd() = NULL (erreur: %s)\n", strerror(errno));
        }
        printf("\n");
    }
    
    /* Test dans /usr/bin */
    if (chdir("/usr/bin") == 0) {
        printf("Répertoire courant: /usr/bin\n");
        if (mon_getcwd(buf, sizeof(buf)) != NULL) {
            printf("mon_getcwd() = '%s'\n", buf);
        } else {
            printf("mon_getcwd() = NULL (erreur: %s)\n", strerror(errno));
        }
        printf("\n");
    }
    
    /* Retourner au répertoire de départ */
    chdir("/home/jnej/Documents/ENSISA/prog_sys/ENSISA-Prog-Syst");
}

/* Test avec allocation automatique */
void test_allocation_automatique(void)
{
    char *result;
    
    printf("=== Test avec allocation automatique ===\n\n");
    
    /* Test avec buf = NULL (allocation automatique) */
    result = mon_getcwd(NULL, 0);
    if (result != NULL) {
        printf("mon_getcwd(NULL, 0) = '%s'\n", result);
        free(result); /* Important : libérer la mémoire allouée */
    } else {
        printf("mon_getcwd(NULL, 0) = NULL (erreur: %s)\n", strerror(errno));
    }
    printf("\n");
}

/* Test de gestion d'erreurs */
void test_gestion_erreurs(void)
{
    printf("=== Test de gestion d'erreurs ===\n\n");
    
    /* Test avec size = 0 */
    char buf[MAX_PATH_LENGTH];
    if (mon_getcwd(buf, 0) == NULL) {
        printf("✓ mon_getcwd(buf, 0) correctement returns NULL (errno = %d)\n", errno);
    } else {
        printf("✗ mon_getcwd(buf, 0) devrait retourner NULL\n");
    }
    
    /* Test avec buffer trop petit */
    char small_buf[5];
    if (mon_getcwd(small_buf, sizeof(small_buf)) == NULL) {
        printf("✓ mon_getcwd(small_buf, 5) correctement returns NULL (errno = %d)\n", errno);
    } else {
        printf("✗ mon_getcwd(small_buf, 5) devrait retourner NULL\n");
    }
    
    printf("\n");
}

int main(void)
{
    printf("=== Exercice 7.5 : Fonction getcwd ===\n\n");
    
    /* Tests de base */
    tester_getcwd();
    
    /* Tests dans différents répertoires */
    tester_dans_differents_repertoires();
    
    /* Test avec allocation automatique */
    test_allocation_automatique();
    
    /* Test de gestion d'erreurs */
    test_gestion_erreurs();
    
    /* Retourner au répertoire de départ */
    chdir("/home/jnej/Documents/ENSISA/prog_sys/ENSISA-Prog-Syst");
    
    printf("=== Fin de l'exercice 7.5 ===\n");
    
    return 0;
}