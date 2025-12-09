#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

typedef struct element {
    char *chaine;
    struct element *suivant;
} element;

element *tete = NULL;

void inserer_trie(char *str) {
    element *nouveau = malloc(sizeof(element));
    nouveau->chaine = malloc(strlen(str) + 1);
    strcpy(nouveau->chaine, str);
    nouveau->suivant = NULL;

    if (tete == NULL || strcmp(tete->chaine, str) >= 0) {
        nouveau->suivant = tete;
        tete = nouveau;
        return;
    }

    element *courant = tete;
    while (courant->suivant != NULL && strcmp(courant->suivant->chaine, str) < 0) {
        courant = courant->suivant;
    }
    nouveau->suivant = courant->suivant;
    courant->suivant = nouveau;
}

void afficher_liste() {
    element *courant = tete;
    while (courant != NULL) {
        printf("%s\n", courant->chaine);
        courant = courant->suivant;
    }
}

int main() {
    char *ligne = NULL;
    size_t len = 0;
    ssize_t read;

    while ((read = getline(&ligne, &len, stdin)) != -1) {
        if (read > 1) { // ignorer les lignes vides
            ligne[strcspn(ligne, "\n")] = 0; // supprimer le \n
            inserer_trie(ligne);
        }
    }

    afficher_liste();

    // libérer la mémoire
    element *courant = tete;
    while (courant != NULL) {
        element *temp = courant;
        courant = courant->suivant;
        free(temp->chaine);
        free(temp);
    }
    free(ligne);

    return 0;
}