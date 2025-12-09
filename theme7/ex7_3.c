/*
 * Exercice 7.3 : Fonction isatty
 * 
 * Il peut arriver qu'un programme désire savoir si l'entrée ou la
 * sortie standard a été redirigée.
 * 
 * Écrivez la fonction isatty qui prend en argument un descripteur
 * de fichier, et renvoie 1 si c'est un terminal (ou plus généralement
 * un périphérique en mode caractère) ou 0 sinon.
 * 
 * Donnez des exemples de programmes dont le comportement dépend
 * de la redirection de l'entrée ou de la sortie standard.
 */

#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>

/* Notre implémentation de isatty */
int mon_isatty(int fd)
{
    struct stat statbuf;
    
    /* Obtenir les informations sur le descripteur */
    if (fstat(fd, &statbuf) < 0) {
        return 0; /* Erreur : considérer comme non-terminal */
    }
    
    /* Vérifier si c'est un périphérique en mode caractère */
    if (S_ISCHR(statbuf.st_mode)) {
        return 1; /* C'est un terminal/périphérique en mode caractère */
    } else {
        return 0; /* Ce n'est pas un terminal */
    }
}

/* Exemples de programmes dont le comportement dépend de la redirection */

/* Exemple 1 : Programme qui adapte son affichage selon la sortie */
void exemple_sortie_adaptative(void)
{
    printf("=== Exemple 1 : Sortie adaptative ===\n");
    
    if (mon_isatty(STDOUT_FILENO)) {
        /* Sortie sur terminal : utiliser des couleurs et formatage */
        printf("\033[1;32m✓ \033[0m");
        printf("Programme exécuté avec succès!\n");
        printf("Vous pouvez utiliser la souris pour sélectionner le texte.\n");
    } else {
        /* Sortie redirigée : format simple */
        printf("[OK] Programme execute avec succes!\n");
        printf("Sortie redirigee vers un fichier.\n");
    }
    printf("\n");
}

/* Exemple 2 : Programme qui demande une confirmation interactive */
void exemple_confirmation_interactive(void)
{
    printf("=== Exemple 2 : Confirmation interactive ===\n");
    
    if (mon_isatty(STDIN_FILENO)) {
        /* Entrée depuis terminal : demander confirmation */
        char reponse[10];
        printf("Voulez-vous continuer ? (o/N): ");
        if (fgets(reponse, sizeof(reponse), stdin) != NULL) {
            if (reponse[0] == 'o' || reponse[0] == 'O') {
                printf("Continuons...\n");
            } else {
                printf("Annulation.\n");
            }
        }
    } else {
        /* Entrée redirigée : continuer automatiquement */
        printf("Mode non-interactif : continuation automatique\n");
    }
    printf("\n");
}

/* Exemple 3 : Programme qui affiche du texte progressif */
void exemple_affichage_progressif(void)
{
    printf("=== Exemple 3 : Affichage progressif ===\n");
    
    if (mon_isatty(STDOUT_FILENO)) {
        /* Terminal : afficher une barre de progression */
        printf("Traitement en cours: [                    ] 0%%\r");
        fflush(stdout);
        
        for (int i = 0; i <= 20; i++) {
            printf("Traitement en cours: [");
            for (int j = 0; j < i; j++) printf("=");
            for (int j = i; j < 20; j++) printf(" ");
            printf("] %d%%\r", i * 5);
            fflush(stdout);
            usleep(100000); /* 100ms */
        }
        printf("\nTermine!\n");
    } else {
        /* Sortie redirigée : affichage simple */
        printf("Debut du traitement...\n");
        for (int i = 0; i <= 20; i++) {
            printf("Progression: %d%%\n", i * 5);
        }
        printf("Termine!\n");
    }
    printf("\n");
}

/* Exemple 4 : Programme qui adapte la taille des colonnes */
void exemple_affichage_tableau(void)
{
    printf("=== Exemple 4 : Affichage tableau adaptatif ===\n");
    
    int colonnes;
    if (mon_isatty(STDOUT_FILENO)) {
        /* Terminal : détecter la largeur */
        colonnes = 80; /* Par défaut, peut être détecté avec ioctl */
        printf("Affichage sur terminal (%d colonnes):\n", colonnes);
    } else {
        /* Sortie redirigée : utiliser une largeur fixe large */
        colonnes = 120;
        printf("Affichage redirige (%d colonnes):\n", colonnes);
    }
    
    /* Simuler l'affichage d'un tableau */
    printf("+");
    for (int i = 0; i < colonnes - 2; i++) printf("-");
    printf("+\n");
    
    printf("| Nom                     | Age | Ville                |");
    if (colonnes > 80) printf(" Profession           |");
    printf("\n");
    
    printf("+");
    for (int i = 0; i < colonnes - 2; i++) printf("-");
    printf("+\n");
    
    printf("| Jean Dupont             | 25  | Paris                |");
    if (colonnes > 80) printf(" Ingenieur           |");
    printf("\n");
    
    printf("| Marie Martin            | 30  | Lyon                 |");
    if (colonnes > 80) printf(" Professor          |");
    printf("\n");
    
    printf("+");
    for (int i = 0; i < colonnes - 2; i++) printf("-");
    printf("+\n");
    printf("\n");
}

/* Test des descripteurs standards */
void tester_descripteurs(void)
{
    printf("=== Test des descripteurs standards ===\n");
    
    printf("STDIN_FILENO  (0) : %s\n", 
           mon_isatty(STDIN_FILENO) ? "terminal" : "redirige/pipe");
    printf("STDOUT_FILENO (1) : %s\n", 
           mon_isatty(STDOUT_FILENO) ? "terminal" : "redirige/pipe");
    printf("STDERR_FILENO (2) : %s\n", 
           mon_isatty(STDERR_FILENO) ? "terminal" : "redirige/pipe");
    printf("\n");
}

/* Fonction principale */
int main(void)
{
    printf("=== Exercice 7.3 : Fonction isatty ===\n\n");
    
    /* Comparer notre implémentation avec la vraie */
    printf("Comparaison des implémentations:\n");
    printf("stdin  : mon_isatty=%d, isatty=%d\n", 
           mon_isatty(STDIN_FILENO), isatty(STDIN_FILENO));
    printf("stdout : mon_isatty=%d, isatty=%d\n", 
           mon_isatty(STDOUT_FILENO), isatty(STDOUT_FILENO));
    printf("stderr : mon_isatty=%d, isatty=%d\n", 
           mon_isatty(STDERR_FILENO), isatty(STDERR_FILENO));
    printf("\n");
    
    /* Tester les descripteurs */
    tester_descripteurs();
    
    /* Exemples de programmes adaptatifs */
    exemple_sortie_adaptative();
    exemple_confirmation_interactive();
    exemple_affichage_progressif();
    exemple_affichage_tableau();
    
    printf("=== Fin de l'exercice 7.3 ===\n");
    
    return 0;
}