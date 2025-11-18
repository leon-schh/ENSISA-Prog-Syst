#include <stdio.h>

#define MAX_LINE 1000
#define LINES 10

int main() {
    char buffer[LINES][MAX_LINE];
    int lengths[LINES] = {0};
    int current = 0;
    int count = 0;
    int index = 0;
    int c;
    while ((c = getchar()) != '\n') {
        if (c == '\n') {
            buffer[current][index] = '\0';
            lengths[current] = index;
            current = (current + 1) % LINES;
            if (count < LINES) count++;
            index = 0;
        } else {
            if (index < MAX_LINE - 1) {
                buffer[current][index++] = c;
            }
        }
    }
    if (index > 0) {
        buffer[current][index] = '\0';
        lengths[current] = index;
        current = (current + 1) % LINES;
        if (count < LINES) count++;
    }
    int start = (current - count + LINES) % LINES;
    for (int i = 0; i < count; i++) {
        int pos = (start + i) % LINES;
        for (int j = 0; j < lengths[pos]; j++) {
            putchar(buffer[pos][j]);
        }
        putchar('\n');
    }
    return 0;
}