#include <stdio.h>

#define PRabp(n) printf("Ligne %d: a: 0x%x %d, b: 0x%x %d, p: 0x%x 0x%x %d\n", \
                        n, &a, a, &b, b, &p, p, *p)
#define PRtp(n) printf("Ligne %d: t: 0x%x %d %d %d, p: 0x%x 0x%x %d\n", \
                       n, t, t[0], t[1], t[2], &p, p, *p)

int main(int argc, char *argv[]) {
    int a, b;
    int t[3] = {4, 5, 6};
    int *p;
    p = &b; a = 0; b = 2; PRabp(1);  // a=0, b=2, p points to b (2)
    *p = 4; PRabp(2);                // b=4, p still points to b
    p++; PRabp(3);                   // p now points to next int after b, undefined behavior
    (*p)++; PRabp(4);                // increments whatever p points to, undefined
    p = 0; PRabp(5);                 // p = NULL, *p will crash
    p = t; PRtp(6);                  // p points to t[0] (4)
    p[0] = 10; p[1] = 11; PRtp(7);  // t[0]=10, t[1]=11
    p++; PRtp(8);                    // p points to t[1] (11)
    *p = 15; *(p + 1) = 16; PRtp(9); // t[1]=15, t[2]=16
    p++; PRtp(10);                   // p points to t[2] (16)
    p[0] = 20; p[1] = 21; PRtp(11); // p[0] is t[2]=20, p[1] is out of bounds, undefined
    return 0;
}