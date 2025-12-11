#include <stdio.h>

/* Structure for testing struct sizes */
struct test_struct {
    char c;
    char *pc;
    int i;
};

int main(void) {
    printf("Sizes on this system:\n");
    printf("int: %zu bytes\n", sizeof(int));
    printf("char: %zu bytes\n", sizeof(char));
    printf("char*: %zu bytes\n", sizeof(char*));
    printf("struct { char c; char *pc; int i; }: %zu bytes\n", sizeof(struct test_struct));
    printf("struct { char c; char *pc; int i; }*: %zu bytes\n", sizeof(struct test_struct*));
    
    printf("\nDetailed analysis of the struct:\n");
    printf("sizeof(char): %zu\n", sizeof(char));
    printf("sizeof(char*): %zu\n", sizeof(char*));
    printf("sizeof(int): %zu\n", sizeof(int));
    
    printf("\nThe struct size (%zu) is greater than the sum of its parts (%zu)\n", 
           sizeof(struct test_struct), 
           sizeof(char) + sizeof(char*) + sizeof(int));
    printf("This is due to padding for alignment requirements.\n");
    
    return 0;
}
