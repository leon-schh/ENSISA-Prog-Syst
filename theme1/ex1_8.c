#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>

struct mois {
    char nom[10];
    int jours;
};

int main() {
    struct mois mois[12] = {
        {"janvier", 31}, {"fevrier", 28}, {"mars", 31}, {"avril", 30},
        {"mai", 31}, {"juin", 30}, {"juillet", 31}, {"aout", 31},
        {"septembre", 30}, {"octobre", 31}, {"novembre", 30}, {"decembre", 31}
    };
    char *line = NULL;
    size_t len = 0;
    ssize_t read = getline(&line, &len, stdin);
    if (read == -1) return 1;
    if (line[read-1] == '\n') line[read-1] = '\0';
    int found = 0;
    for (int i = 0; i < 12; i++) {
        if (strcmp(line, mois[i].nom) == 0) {
            printf("%d %d\n", i+1, mois[i].jours);
            found = 1;
            break;
        }
    }
    if (!found) {
        puts("Mois invalide");
    }
    free(line);
    return 0;
}