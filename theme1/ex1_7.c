#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>

int main() {
    char *line1 = NULL, *line2 = NULL;
    size_t len1 = 0, len2 = 0;
    ssize_t read1 = getline(&line1, &len1, stdin);
    ssize_t read2 = getline(&line2, &len2, stdin);
    if (read1 == -1 || read2 == -1) return 1;
    // Remove \n
    if (line1[read1-1] == '\n') line1[read1-1] = '\0';
    if (line2[read2-1] == '\n') line2[read2-1] = '\0';
    size_t l1 = strlen(line1);
    size_t l2 = strlen(line2);
    int found = 0;
    for (size_t i = 0; i <= l1 - l2; i++) {
        int match = 1;
        for (size_t j = 0; j < l2; j++) {
            if (line1[i + j] != line2[j]) {
                match = 0;
                break;
            }
        }
        if (match) {
            found = 1;
            break;
        }
    }
    if (found) {
        puts("Oui");
    } else {
        puts("Non");
    }
    free(line1);
    free(line2);
    return 0;
}