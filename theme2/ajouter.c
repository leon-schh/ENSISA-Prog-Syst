#include "ex2_3.h"
#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: ajouter name tel\n");
        return 1;
    }
    FILE *f = fopen("carnet", "r+");
    if (!f) {
        f = fopen("carnet", "w+");
        if (!f) {
            perror("fopen");
            return 1;
        }
    }
    fiche fic;
    long pos = 0;
    int found = 0;
    while (fread(&fic, sizeof(fiche), 1, f) == 1) {
        if (!fic.occupe) {
            found = 1;
            break;
        }
        pos += sizeof(fiche);
    }
    if (!found) {
        fseek(f, 0, SEEK_END);
    } else {
        fseek(f, pos, SEEK_SET);
    }
    fic.occupe = 1;
    strcpy(fic.nom, argv[1]);
    strcpy(fic.telephone, argv[2]);
    fwrite(&fic, sizeof(fiche), 1, f);
    fclose(f);
    return 0;
}