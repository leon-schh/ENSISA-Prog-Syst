/*
 * Exercice 7.2 : Fonction system
 * 
 * La fonction de librairie system prend en argument une commande
 * (contenant éventuellement des redirections, ou même composée de
 * plusieurs commandes reliées par un tube), l'exécute et renvoie
 * le code de retour de la commande si elle a été lancée, ou -1 sinon.
 * 
 * On utilise le Shell de Bourne (/bin/sh) avec l'option -c
 * pour exécuter la commande.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>

/* Notre implémentation de la fonction system */
int mon_system(const char *commande)
{
    pid_t pid;
    int status;
    
    /* Vérifier que la commande n'est pas NULL */
    if (commande == NULL) {
        return -1;
    }
    
    /* Créer un processus fils */
    pid = fork();
    
    if (pid < 0) {
        /* Erreur lors du fork */
        perror("fork");
        return -1;
    }
    
    if (pid == 0) {
        /* Processus fils : exécuter la commande avec /bin/sh */
        execl("/bin/sh", "sh", "-c", commande, (char *)NULL);
        
        /* Si execl échoue, on sort avec -1 */
        perror("execl");
        exit(-1);
    } else {
        /* Processus père : attendre la terminaison du fils */
        while (waitpid(pid, &status, 0) < 0) {
            if (errno != EINTR) {
                /* Erreur autre qu'une interruption par un signal */
                return -1;
            }
        }
        
        /* Vérifier comment le processus fils s'est terminé */
        if (WIFEXITED(status)) {
            /* Terminaison normale : retourner le code de retour */
            return WEXITSTATUS(status);
        } else if (WIFSIGNALED(status)) {
            /* Terminaison par un signal : retourner le numéro du signal + 128 */
            return 128 + WTERMSIG(status);
        } else {
            /* Autres cas (arrêt, continuation) */
            return -1;
        }
    }
}

/* Fonction de test pour comparer avec la vraie fonction system */
void tester_system(void)
{
    int resultat_mon_system, resultat_vrai_system;
    
    printf("=== Comparaison des fonctions system ===\n\n");
    
    /* Test 1 : Commande simple qui réussit */
    printf("Test 1 : echo 'Hello World'\n");
    resultat_mon_system = mon_system("echo 'Hello World'");
    resultat_vrai_system = system("echo 'Hello World'");
    printf("mon_system() a retourné : %d\n", resultat_mon_system);
    printf("system() a retourné    : %d\n\n", resultat_vrai_system);
    
    /* Test 2 : Commande qui échoue */
    printf("Test 2 : ls /inexistant\n");
    resultat_mon_system = mon_system("ls /inexistant");
    resultat_vrai_system = system("ls /inexistant");
    printf("mon_system() a retourné : %d\n", resultat_mon_system);
    printf("system() a retourné    : %d\n\n", resultat_vrai_system);
    
    /* Test 3 : Commande avec redirection */
    printf("Test 3 : echo 'test' > /tmp/test_system.txt\n");
    resultat_mon_system = mon_system("echo 'test' > /tmp/test_system.txt && cat /tmp/test_system.txt");
    resultat_vrai_system = system("echo 'test' > /tmp/test_system.txt && cat /tmp/test_system.txt");
    printf("mon_system() a retourné : %d\n", resultat_mon_system);
    printf("system() a retourné    : %d\n\n", resultat_vrai_system);
    
    /* Test 4 : Commande avec tube */
    printf("Test 4 : echo -e 'ligne1\\nligne2\\nligne3' | wc -l\n");
    resultat_mon_system = mon_system("echo -e 'ligne1\\nligne2\\nligne3' | wc -l");
    resultat_vrai_system = system("echo -e 'ligne1\\nligne2\\nligne3' | wc -l");
    printf("mon_system() a retourné : %d\n", resultat_mon_system);
    printf("system() a retourné    : %d\n\n", resultat_vrai_system);
    
    /* Test 5 : Commande vide */
    printf("Test 5 : commande NULL\n");
    resultat_mon_system = mon_system(NULL);
    printf("mon_system(NULL) a retourné : %d\n", resultat_mon_system);
    
    /* Test 6 : Commande chaîne vide */
    printf("Test 6 : chaîne vide\n");
    resultat_mon_system = mon_system("");
    printf("mon_system('') a retourné : %d\n", resultat_mon_system);
}

int main(void)
{
    printf("=== Exercice 7.2 : Fonction system ===\n\n");
    
    tester_system();
    
    return 0;
}