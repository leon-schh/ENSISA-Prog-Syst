#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <ctype.h>

int main() {
    char *line = NULL;
    size_t len = 0;
    ssize_t read = getline(&line, &len, stdin);
    if (read == -1) return 1;
    char *output = malloc(read + 1);
    int out_index = 0;
    for (int i = 0; line[i] != '\0'; i++) {
        if (line[i] == '\\') {
            i++;
            if (line[i] == 'n') output[out_index++] = '\n';
            else if (line[i] == 'r') output[out_index++] = '\r';
            else if (line[i] == 't') output[out_index++] = '\t';
            else if (line[i] == 'b') output[out_index++] = '\b';
            else if (line[i] == '\\') output[out_index++] = '\\';
            else if (line[i] == '\'') output[out_index++] = '\'';
            else if (line[i] == '\"') output[out_index++] = '\"';
            else if (isdigit(line[i])) {
                int val = 0;
                for (int j = 0; j < 3 && isdigit(line[i+j]); j++) {
                    val = val * 8 + (line[i+j] - '0');
                }
                output[out_index++] = val;
                i += 2; 
            } else {
                output[out_index++] = line[i];
            }
        } else {
            output[out_index++] = line[i];
        }
    }
    output[out_index] = '\0';
    puts(output);
    free(line);
    free(output);
    return 0;
}