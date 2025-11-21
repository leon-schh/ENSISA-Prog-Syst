#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
    int count = 0;
    char target = '\n';
    if (argc > 1) {
        if (strlen(argv[1]) != 1) {
            fprintf(stderr, "Argument must be a single character\n");
            return 1;
        }
        target = argv[1][0];
    }
    int c;
    while ((c = getchar()) != EOF) {
        if (c == target) count++;
    }
    printf("%d\n", count);
    return 0;
}