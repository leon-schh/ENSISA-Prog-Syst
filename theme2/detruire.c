#include "ex2_3.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: detruire num\n");
        return 1;
    }
    int num = atoi(argv[1]);
    FILE *f = fopen("carnet", "r+");
    if (!f) {
        perror("fopen");
        return 1;
    }
    fiche fic;
    fseek(f, num * sizeof(fiche), SEEK_SET);
    if (fread(&fic, sizeof(fiche), 1, f) == 1) {
        fic.occupe = 0;
        fseek(f, num * sizeof(fiche), SEEK_SET);
        fwrite(&fic, sizeof(fiche), 1, f);
    }
    fclose(f);
    return 0;
}