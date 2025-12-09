#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    char chaine1[256];
    char chaine2[256];

    printf("Entrez la première chaîne: ");
    fgets(chaine1, sizeof(chaine1), stdin);
    chaine1[strcspn(chaine1, "\n")] = 0;

    printf("Entrez la deuxième chaîne: ");
    fgets(chaine2, sizeof(chaine2), stdin);
    chaine2[strcspn(chaine2, "\n")] = 0;

    size_t len1 = strlen(chaine1);
    size_t len2 = strlen(chaine2);

    if (len2 > len1) {
        printf("La deuxième chaîne n'est pas dans la première.\n");
        return 0;
    }

    for (size_t i = 0; i <= len1 - len2; i++) {
        if (strncmp(chaine1 + i, chaine2, len2) == 0) {
            printf("La deuxième chaîne est trouvée à la position %zu.\n", i);
            return 0;
        }
    }

    printf("La deuxième chaîne n'est pas dans la première.\n");
    return 0;
}