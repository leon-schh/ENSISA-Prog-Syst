#include <stdio.h>

int main() {
    FILE *src = fopen("toto", "r");
    if (!src) {
        perror("fopen toto");
        return 1;
    }
    FILE *dst = fopen("titi", "w");
    if (!dst) {
        perror("fopen titi");
        fclose(src);
        return 1;
    }
    int c;
    while ((c = getc(src)) != EOF) {
        putc(c, dst);
    }
    fclose(src);
    fclose(dst);
    return 0;
}