#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage: archiver archive file...\n");
        return 1;
    }
    char *archive = argv[1];
    FILE *out = fopen(archive, "wb");
    if (!out) {
        perror("fopen archive");
        return 1;
    }
    for (int i = 2; i < argc; i++) {
        FILE *in = fopen(argv[i], "rb");
        if (!in) {
            perror(argv[i]);
            continue;
        }
        fseek(in, 0, SEEK_END);
        long size = ftell(in);
        fseek(in, 0, SEEK_SET);
        int namelen = strlen(argv[i]) + 1;
        fwrite(&namelen, sizeof(int), 1, out);
        fwrite(argv[i], 1, namelen, out);
        fwrite(&size, sizeof(long), 1, out);
        char buf[1024];
        size_t n;
        while ((n = fread(buf, 1, sizeof(buf), in)) > 0) {
            fwrite(buf, 1, n, out);
        }
        fclose(in);
    }
    fclose(out);
    return 0;
}