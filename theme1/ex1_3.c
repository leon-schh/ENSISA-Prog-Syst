#include <stdio.h>
#include <ctype.h>

int main() {
    int word_count = 0;
    int in_word = 0;
    int c;
    while ((c = getchar()) != '\n') {
        if (isalpha(c)) {
            if (!in_word) {
                in_word = 1;
                word_count++;
            }
        } else {
            in_word = 0;
        }
    }
    printf("%d\n", word_count);
    return 0;
}