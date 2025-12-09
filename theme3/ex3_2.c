#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

typedef struct element {
    int valeur;
    struct element *suivant;
} element;

element *tete = NULL;

void inserer_trie(int val) {
    element *nouveau = malloc(sizeof(element));
    nouveau->valeur = val;
    nouveau->suivant = NULL;

    if (tete == NULL || tete->valeur >= val) {
        nouveau->suivant = tete;
        tete = nouveau;
        return;
    }

    element *courant = tete;
    while (courant->suivant != NULL && courant->suivant->valeur < val) {
        courant = courant->suivant;
    }
    nouveau->suivant = courant->suivant;
    courant->suivant = nouveau;
}

void afficher_liste() {
    element *courant = tete;
    while (courant != NULL) {
        printf("%d ", courant->valeur);
        courant = courant->suivant;
    }
    printf("\n");
}

int main() {
    char *ligne = NULL;
    size_t len = 0;
    ssize_t read;

    while ((read = getline(&ligne, &len, stdin)) != -1) {
        if (read > 1) { // ignorer les lignes vides
            int val = atoi(ligne);
            inserer_trie(val);
        }
    }

    afficher_liste();

    // libérer la mémoire
    element *courant = tete;
    while (courant != NULL) {
        element *temp = courant;
        courant = courant->suivant;
        free(temp);
    }
    free(ligne);

    return 0;
}