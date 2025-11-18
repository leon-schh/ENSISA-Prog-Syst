#include <stdio.h>
#include <ctype.h>

int main() {
    int counts[26] = {0};
    int c;
    while ((c = getchar()) != '\n') {
        if (isalpha(c)) {
            c = tolower(c);
            counts[c - 'a']++;
        }
    }
    for (int i = 0; i < 26; i++) {
        if (counts[i] > 0) {
            printf("%c: %d\n", 'a' + i, counts[i]);
        }
    }
    return 0;
}