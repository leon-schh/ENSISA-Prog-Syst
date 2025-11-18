#include <stdio.h>

int main() {
    for (int i = 0; i < 10; i++) {
        printf("%d\n", 1 << i);
    }
    return 0;
}