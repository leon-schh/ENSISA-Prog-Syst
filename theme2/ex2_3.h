#define MAXNOM 50
#define MAXTEL 10
struct fiche {
    int occupe; // vrai si la fiche est utilisée
    char nom[MAXNOM]; // le nom de la personne
    char telephone[MAXTEL]; // son numéro de téléphone
};
typedef struct fiche fiche;