#include <stdio.h>

#define ADRESSE_RELATIVE_DE(s, c) ((size_t)&(((s *)0)->c))

struct test {
    char c;
    int i;
    double d;
};

int main() {
    printf("Adresse relative de c: %zu\n", ADRESSE_RELATIVE_DE(struct test, c));
    printf("Adresse relative de i: %zu\n", ADRESSE_RELATIVE_DE(struct test, i));
    printf("Adresse relative de d: %zu\n", ADRESSE_RELATIVE_DE(struct test, d));
    return 0;
}