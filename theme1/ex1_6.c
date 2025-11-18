#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>

int main() {
    char *line = NULL;
    size_t len = 0;
    ssize_t read = getline(&line, &len, stdin);
    if (read == -1) return 1;
    char *copy = malloc(strlen(line) + 1);
    strcpy(copy, line);
    puts(copy);
    printf("%zu\n", strlen(copy));
    free(line);
    free(copy);
    return 0;
}