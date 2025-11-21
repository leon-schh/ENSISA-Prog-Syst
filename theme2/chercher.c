#include "ex2_3.h"
#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: chercher name\n");
        return 1;
    }
    FILE *f = fopen("carnet", "r");
    if (!f) {
        perror("fopen");
        return 1;
    }
    fiche fic;
    int num = 0;
    while (fread(&fic, sizeof(fiche), 1, f) == 1) {
        if (fic.occupe && strcmp(fic.nom, argv[1]) == 0) {
            printf("%d\n", num);
            fclose(f);
            return 0;
        }
        num++;
    }
    fclose(f);
    return 0;
}