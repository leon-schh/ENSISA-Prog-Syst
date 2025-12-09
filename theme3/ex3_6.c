#include <stdio.h>

char *mon_strchr(char *chaine, int car) {
    while (*chaine != '\0') {
        if (*chaine == car) {
            return chaine;
        }
        chaine++;
    }
    return NULL;
}

int main() {
    char *test = "Hello World";
    char *result = mon_strchr(test, 'o');
    if (result) {
        printf("Trouvé: %s\n", result);
    } else {
        printf("Non trouvé\n");
    }
    return 0;
}