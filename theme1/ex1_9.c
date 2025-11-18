#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>

int main() {
    char *line = NULL;
    size_t len = 0;
    ssize_t read = getline(&line, &len, stdin);
    if (read == -1) return 1;
    char *output = malloc(2 * read + 1);
    int out_index = 0;
    for (int i = 0; line[i] != '\0'; i++) {
        char c = line[i];
        if (c >= 32 && c <= 126 && c != '\\' && c != '\'' && c != '\"') {
            output[out_index++] = c;
        } else {
            output[out_index++] = '\\';
            if (c == '\n') output[out_index++] = 'n';
            else if (c == '\r') output[out_index++] = 'r';
            else if (c == '\t') output[out_index++] = 't';
            else if (c == '\b') output[out_index++] = 'b';
            else if (c == '\\') output[out_index++] = '\\';
            else if (c == '\'') output[out_index++] = '\'';
            else if (c == '\"') output[out_index++] = '\"';
            else {
                output[out_index++] = (c / 64) + '0';
                output[out_index++] = ((c / 8) % 8) + '0';
                output[out_index++] = (c % 8) + '0';
            }
        }
    }
    output[out_index] = '\0';
    puts(output);
    free(line);
    free(output);
    return 0;
}