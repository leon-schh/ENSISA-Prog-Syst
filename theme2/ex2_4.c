#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
extern char *optarg;
extern int optind;

int main(int argc, char *argv[]) {
    int n = 10;
    int opt;
    while ((opt = getopt(argc, argv, "n:")) != -1) {
        if (opt == 'n') {
            n = atoi(optarg);
        }
    }
    if (optind >= argc) {
        fprintf(stderr, "Usage: tail [-n num] file\n");
        return 1;
    }
    char *filename = argv[optind];
    FILE *f = fopen(filename, "r");
    if (!f) {
        perror("fopen");
        return 1;
    }
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    long pos = size;
    int lines_found = 0;
    const int bufsize = 1024;
    char buf[bufsize];
    while (pos > 0 && lines_found <= n) {
        long to_read = pos > bufsize ? bufsize : pos;
        pos -= to_read;
        fseek(f, pos, SEEK_SET);
        fread(buf, 1, to_read, f);
        for (int i = to_read - 1; i >= 0; i--) {
            if (buf[i] == '\n') {
                lines_found++;
                if (lines_found > n) {
                    pos += i + 1;
                    goto found;
                }
            }
        }
    }
    pos = 0;
found:
    fseek(f, pos, SEEK_SET);
    int c;
    while ((c = getc(f)) != EOF) {
        putchar(c);
    }
    fclose(f);
    return 0;
}