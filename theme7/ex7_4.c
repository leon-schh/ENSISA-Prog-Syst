#define _GNU_SOURCE
/*
 * Exercice 7.4 : Fonction my_ttyname
 * 
 * Les champs st_dev et st_ino de la structure stat identifient de
 * manière unique un fichier dans le système.
 * 
 * Écrivez une fonction my_ttyname analogue à la fonction de librairie
 * ttyname qui prend en paramètre un descripteur de fichier, et renvoie
 * un pointeur sur une chaîne (statique) contenant le nom complet du
 * fichier correspondant (cherché dans /dev), ou NULL si le fichier
 * n'est pas trouvé.
 * 
 * Sur Linux, on cherchera dans /dev/pts.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>

#define MAX_DEV_DIRS 3
static const char *dev_dirs[] = {
    "/dev",
    "/dev/pts",
    "/dev/shm"
};

#define MAX_PATH_LENGTH 256

/* Notre implémentation de ttyname */
char *mon_ttyname(int fd)
{
    static char tty_name[MAX_PATH_LENGTH];
    struct stat fd_stat, dev_stat;
    DIR *dir;
    struct dirent *entry;
    int i;
    
    /* Obtenir les informations sur le descripteur de fichier */
    if (fstat(fd, &fd_stat) < 0) {
        return NULL;
    }
    
    /* Vérifier que c'est bien un périphérique en mode caractère */
    if (!S_ISCHR(fd_stat.st_mode)) {
        return NULL;
    }
    
    /* Chercher dans chaque répertoire /dev */
    for (i = 0; i < MAX_DEV_DIRS; i++) {
        dir = opendir(dev_dirs[i]);
        if (dir == NULL) {
            continue; /* Répertoire non accessible, passer au suivant */
        }
        
        /* Parcourir tous les fichiers du répertoire */
        while ((entry = readdir(dir)) != NULL) {
            /* Construire le chemin complet */
            snprintf(tty_name, sizeof(tty_name), "%s/%s", 
                    dev_dirs[i], entry->d_name);
            
            /* Obtenir les informations sur ce fichier */
            if (stat(tty_name, &dev_stat) < 0) {
                continue;
            }
            
            /* Vérifier si c'est le même périphérique */
            if (S_ISCHR(dev_stat.st_mode) &&
                dev_stat.st_rdev == fd_stat.st_rdev) {
                /* Trouvé ! */
                closedir(dir);
                return tty_name;
            }
        }
        
        closedir(dir);
    }
    
    /* Non trouvé */
    return NULL;
}

/* Fonction pour afficher les informations d'un périphérique */
void afficher_info_peripherique(int fd, const char *nom)
{
    struct stat statbuf;
    
    printf("=== Informations sur %s (descripteur %d) ===\n", nom, fd);
    
    if (fstat(fd, &statbuf) < 0) {
        printf("Erreur lors de fstat: %s\n", strerror(errno));
        return;
    }
    
    printf("Mode: 0%o\n", statbuf.st_mode);
    printf("Type: %s\n", S_ISCHR(statbuf.st_mode) ? "Périphérique caractères" : 
                                 S_ISBLK(statbuf.st_mode) ? "Périphérique bloc" :
                                 S_ISDIR(statbuf.st_mode) ? "Répertoire" :
                                 S_ISREG(statbuf.st_mode) ? "Fichier régulier" :
                                 "Autre");
    
    if (S_ISCHR(statbuf.st_mode) || S_ISBLK(statbuf.st_mode)) {
        printf("Numéro majeur: %d\n", (int)(((statbuf.st_rdev) >> 8) & 0xff));
        printf("Numéro mineur: %d\n", (int)((statbuf.st_rdev) & 0xff));
    }
    
    printf("Inode: %ld\n", (long)statbuf.st_ino);
    printf("Taille: %ld octets\n", (long)statbuf.st_size);
    printf("\n");
}

/* Test de la fonction my_ttyname */
void tester_ttyname(void)
{
    char *nom;
    
    printf("=== Test de mon_ttyname ===\n\n");
    
    /* Tester avec stdin */
    printf("Test avec STDIN_FILENO (0):\n");
    nom = mon_ttyname(STDIN_FILENO);
    if (nom != NULL) {
        printf("mon_ttyname(STDIN_FILENO) = '%s'\n", nom);
    } else {
        printf("mon_ttyname(STDIN_FILENO) = NULL\n");
    }
    
    /* Comparer avec la vraie fonction ttyname */
    nom = ttyname(STDIN_FILENO);
    if (nom != NULL) {
        printf("ttyname(STDIN_FILENO)      = '%s'\n", nom);
    } else {
        printf("ttyname(STDIN_FILENO)      = NULL\n");
    }
    printf("\n");
    
    /* Tester avec stdout */
    printf("Test avec STDOUT_FILENO (1):\n");
    nom = mon_ttyname(STDOUT_FILENO);
    if (nom != NULL) {
        printf("mon_ttyname(STDOUT_FILENO) = '%s'\n", nom);
    } else {
        printf("mon_ttyname(STDOUT_FILENO) = NULL\n");
    }
    
    nom = ttyname(STDOUT_FILENO);
    if (nom != NULL) {
        printf("ttyname(STDOUT_FILENO)     = '%s'\n", nom);
    } else {
        printf("ttyname(STDOUT_FILENO)     = NULL\n");
    }
    printf("\n");
    
    /* Tester avec stderr */
    printf("Test avec STDERR_FILENO (2):\n");
    nom = mon_ttyname(STDERR_FILENO);
    if (nom != NULL) {
        printf("mon_ttyname(STDERR_FILENO) = '%s'\n", nom);
    } else {
        printf("mon_ttyname(STDERR_FILENO) = NULL\n");
    }
    
    nom = ttyname(STDERR_FILENO);
    if (nom != NULL) {
        printf("ttyname(STDERR_FILENO)     = '%s'\n", nom);
    } else {
        printf("ttyname(STDERR_FILENO)     = NULL\n");
    }
    printf("\n");
}

/* Fonction pour lister les périphériques de caractères disponibles */
void lister_peripheriques_caracteres(void)
{
    DIR *dir;
    struct dirent *entry;
    struct stat statbuf;
    int count = 0;
    
    printf("=== Périphériques de caractères dans /dev ===\n");
    
    dir = opendir("/dev");
    if (dir == NULL) {
        printf("Erreur lors de l'ouverture de /dev: %s\n", strerror(errno));
        return;
    }
    
    printf("Périphériques de caractères trouvés:\n");
    
    while ((entry = readdir(dir)) != NULL && count < 20) {
        char path[MAX_PATH_LENGTH];
        snprintf(path, sizeof(path), "/dev/%s", entry->d_name);
        
        if (stat(path, &statbuf) == 0 && S_ISCHR(statbuf.st_mode)) {
            printf("  %s (majeur=%d, mineur=%d)\n",
                   entry->d_name,
                   (int)(((statbuf.st_rdev) >> 8) & 0xff),
                   (int)((statbuf.st_rdev) & 0xff));
            count++;
        }
    }
    
    if (count == 20) {
        printf("  ... et plus (limité à 20 pour l'affichage)\n");
    }
    
    closedir(dir);
    printf("\n");
}

/* Test avec un descripteur de fichier régulier */
void test_avec_fichier_regulier(void)
{
    FILE *fichier;
    char *nom;
    
    printf("=== Test avec un fichier régulier ===\n");
    
    fichier = fopen("/etc/passwd", "r");
    if (fichier == NULL) {
        printf("Erreur lors de l'ouverture de /etc/passwd\n");
        return;
    }
    
    printf("Test avec le descripteur de /etc/passwd:\n");
    nom = mon_ttyname(fileno(fichier));
    if (nom != NULL) {
        printf("mon_ttyname(fileno(fichier)) = '%s'\n", nom);
    } else {
        printf("mon_ttyname(fileno(fichier)) = NULL (attendu)\n");
    }
    
    nom = ttyname(fileno(fichier));
    if (nom != NULL) {
        printf("ttyname(fileno(fichier))     = '%s'\n", nom);
    } else {
        printf("ttyname(fileno(fichier))     = NULL (attendu)\n");
    }
    
    fclose(fichier);
    printf("\n");
}

int main(void)
{
    printf("=== Exercice 7.4 : Fonction my_ttyname ===\n\n");
    
    /* Afficher les informations sur les descripteurs */
    afficher_info_peripherique(STDIN_FILENO, "STDIN");
    afficher_info_peripherique(STDOUT_FILENO, "STDOUT");
    afficher_info_peripherique(STDERR_FILENO, "STDERR");
    
    /* Lister les périphériques de caractères */
    lister_peripheriques_caracteres();
    
    /* Tester la fonction ttyname */
    tester_ttyname();
    
    /* Test avec fichier régulier */
    test_avec_fichier_regulier();
    
    printf("=== Fin de l'exercice 7.4 ===\n");
    
    return 0;
}