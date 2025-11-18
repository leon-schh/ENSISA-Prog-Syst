#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <ctype.h>

unsigned int lire_16(void) {
    char *line = NULL;
    size_t len = 0;
    ssize_t read = getline(&line, &len, stdin);
    if (read == -1) return 0;
    if (line[read-1] == '\n') line[read-1] = '\0';
    unsigned int value = 0;
    for (int i = 0; line[i] != '\0'; i++) {
        char c = line[i];
        int digit;
        if (isdigit(c)) digit = c - '0';
        else if (c >= 'a' && c <= 'f') digit = 10 + (c - 'a');
        else if (c >= 'A' && c <= 'F') digit = 10 + (c - 'A');
        else continue; 
        value = (value << 4) | digit;
    }
    free(line);
    return value;
}

int main() {
    unsigned int val = lire_16();
    printf("%u\n", val);
    return 0;
}