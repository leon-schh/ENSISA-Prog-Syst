#include <stdio.h>

void ecrire_2(unsigned int n) {
    for (int i = 31; i >= 0; i--) {
        putchar((n & (1U << i)) ? '1' : '0');
    }
    putchar('\n');
}

int main() {
    unsigned int val;
    scanf("%u", &val);
    ecrire_2(val);
    return 0;
}