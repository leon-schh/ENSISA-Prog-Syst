#include "ex2_3.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: afficher num\n");
        return 1;
    }
    int num = atoi(argv[1]);
    FILE *f = fopen("carnet", "r");
    if (!f) {
        perror("fopen");
        return 1;
    }
    fiche fic;
    fseek(f, num * sizeof(fiche), SEEK_SET);
    if (fread(&fic, sizeof(fiche), 1, f) == 1 && fic.occupe) {
        printf("%s %s\n", fic.nom, fic.telephone);
    }
    fclose(f);
    return 0;
}