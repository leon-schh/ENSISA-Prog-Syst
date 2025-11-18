#include <stdio.h>
#include <ctype.h>
#include <string.h>

int main() {
    char word[100]; // assume words < 100 chars
    int index = 0;
    int in_word = 0;
    int c;
    while ((c = getchar()) != '\n') {
        if (isalpha(c)) {
            if (!in_word) {
                in_word = 1;
                index = 0;
            }
            word[index++] = c;
        } else {
            if (in_word) {
                word[index] = '\0';
                puts(word);
                in_word = 0;
            }
        }
    }
    if (in_word) {
        word[index] = '\0';
        puts(word);
    }
    return 0;
}