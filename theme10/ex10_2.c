#include <stdio.h>
#include <stddef.h>

/* Structure for testing */
struct test_struct {
    char c;
    char *pc;
    int i;
    double d;
};

/* Macro to get the relative address of a field in a struct */
#define ADRESSE_RELATIVE_DE(s, c) ((size_t)&((s*)0)->c)

int main(void) {
    struct test_struct v;
    
    printf("Relative addresses of struct fields:\n");
    printf("Address of v: %p\n", (void*)&v);
    
    /* Display the relative address of each field */
    printf("Relative address of field 'c': %zu bytes\n", ADRESSE_RELATIVE_DE(struct test_struct, c));
    printf("Relative address of field 'pc': %zu bytes\n", ADRESSE_RELATIVE_DE(struct test_struct, pc));
    printf("Relative address of field 'i': %zu bytes\n", ADRESSE_RELATIVE_DE(struct test_struct, i));
    printf("Relative address of field 'd': %zu bytes\n", ADRESSE_RELATIVE_DE(struct test_struct, d));
    
    /* Verify with actual addresses */
    printf("\nVerification with actual addresses:\n");
    printf("Actual address of 'c': %zu (offset: %zu)\n", 
           (size_t)&v.c, (size_t)&v.c - (size_t)&v);
    printf("Actual address of 'pc': %zu (offset: %zu)\n", 
           (size_t)&v.pc, (size_t)&v.pc - (size_t)&v);
    printf("Actual address of 'i': %zu (offset: %zu)\n", 
           (size_t)&v.i, (size_t)&v.i - (size_t)&v);
    printf("Actual address of 'd': %zu (offset: %zu)\n", 
           (size_t)&v.d, (size_t)&v.d - (size_t)&v);
    
    return 0;
}
