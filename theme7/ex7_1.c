/*
 * Exercice 7.1 : Environnement d'un processus
 * 
 * La variable globale environ est déclarée comme :
 * extern char **environ
 * 
 * Cette variable référence le premier élément d'un tableau de chaînes
 * de la forme var=valeur. Ce tableau est terminé par le pointeur NULL.
 * 
 * Structure de données :
 * 
 * environ -> ["PATH=/bin:/usr/bin", "HOME=/home/user", "USER=john", NULL]
 *             ^                  ^                ^            ^
 *          [0]                [1]               [2]         [3]
 * 
 * Chaque élément est une chaîne de caractères terminée par '\0'
 */

#include <stdio.h>
#include <string.h>

/* Déclaration externe de environ */
extern char **environ;

/* Fonction getenv qui recherche une variable d'environnement */
char *mon_getenv(const char *nom)
{
    int i;
    int len = strlen(nom);
    
    /* Parcourir le tableau environ */
    for (i = 0; environ[i] != NULL; i++) {
        /* Vérifier si la chaîne commence par le nom demandé suivi de '=' */
        if (strncmp(environ[i], nom, len) == 0 && environ[i][len] == '=') {
            /* Retourner un pointeur vers la valeur (après le '=') */
            return environ[i] + len + 1;
        }
    }
    
    /* Variable non trouvée */
    return NULL;
}

/* Fonction pour afficher la structure environ */
void afficher_environ(void)
{
    int i;
    
    printf("Structure de données environ :\n");
    printf("environ -> [");
    
    for (i = 0; environ[i] != NULL; i++) {
        printf("\"%s\"", environ[i]);
        if (environ[i + 1] != NULL) {
            printf(", ");
        }
    }
    printf(", NULL]\n\n");
}

/* Fonction de test */
int main(void)
{
    char *path, *home, *user, *inexistant;
    
    printf("=== Exercice 7.1 : Environnement d'un processus ===\n\n");
    
    /* Afficher la structure environ */
    afficher_environ();
    
    /* Tester la fonction mon_getenv */
    printf("Tests de mon_getenv :\n");
    
    path = mon_getenv("PATH");
    if (path != NULL) {
        printf("PATH = %s\n", path);
    } else {
        printf("PATH non trouvé\n");
    }
    
    home = mon_getenv("HOME");
    if (home != NULL) {
        printf("HOME = %s\n", home);
    } else {
        printf("HOME non trouvé\n");
    }
    
    user = mon_getenv("USER");
    if (user != NULL) {
        printf("USER = %s\n", user);
    } else {
        printf("USER non trouvé\n");
    }
    
    inexistant = mon_getenv("VARIABLE_INEXISTANTE");
    if (inexistant != NULL) {
        printf("VARIABLE_INEXISTANTE = %s\n", inexistant);
    } else {
        printf("VARIABLE_INEXISTANTE non trouvé\n");
    }
    
    return 0;
}