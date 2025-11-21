#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: desarchiver archive\n");
        return 1;
    }
    FILE *in = fopen(argv[1], "rb");
    if (!in) {
        perror("fopen");
        return 1;
    }
    while (1) {
        int namelen;
        if (fread(&namelen, sizeof(int), 1, in) != 1) break;
        char *name = malloc(namelen);
        if (fread(name, 1, namelen, in) != namelen) {
            free(name);
            break;
        }
        long size;
        if (fread(&size, sizeof(long), 1, in) != 1) {
            free(name);
            break;
        }
        FILE *out = fopen(name, "wb");
        if (!out) {
            perror(name);
            free(name);
            continue;
        }
        char buf[1024];
        long remaining = size;
        while (remaining > 0) {
            size_t to_read = remaining > sizeof(buf) ? sizeof(buf) : remaining;
            size_t n = fread(buf, 1, to_read, in);
            if (n == 0) break;
            fwrite(buf, 1, n, out);
            remaining -= n;
        }
        fclose(out);
        free(name);
    }
    fclose(in);
    return 0;
}