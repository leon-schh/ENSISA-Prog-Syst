#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

/* Function to find the first line in memory-mapped file */
char* find_first_line(const char *data, size_t size) {
    if (size == 0) {
        return NULL;
    }
    
    /* Find the first newline or end of file */
    size_t line_length = 0;
    for (size_t i = 0; i < size; i++) {
        if (data[i] == '\n') {
            line_length = i;
            break;
        }
        line_length = i + 1;
    }
    
    if (line_length == 0) {
        return NULL;
    }
    
    /* Allocate memory for the line and copy it */
    char *line = (char*)malloc(line_length + 1);
    if (line == NULL) {
        return NULL;
    }
    
    memcpy(line, data, line_length);
    line[line_length] = '\0';
    
    return line;
}

/* Function to find the last line in memory-mapped file */
char* find_last_line(const char *data, size_t size) {
    if (size == 0) {
        return NULL;
    }
    
    /* Start from the end and go backwards to find the last newline */
    size_t line_start = size - 1;
    size_t line_length = 1;
    
    /* Move backwards until we find a newline or reach the beginning */
    while (line_start > 0 && data[line_start - 1] != '\n') {
        line_start--;
        line_length++;
    }
    
    /* Allocate memory for the line and copy it */
    char *line = (char*)malloc(line_length + 1);
    if (line == NULL) {
        return NULL;
    }
    
    memcpy(line, data + line_start, line_length);
    line[line_length] = '\0';
    
    /* Remove trailing newline if present */
    if (line_length > 0 && line[line_length - 1] == '\n') {
        line[line_length - 1] = '\0';
    }
    
    return line;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        return 1;
    }
    
    const char *filename = argv[1];
    
    /* Open the file */
    int fd = open(filename, O_RDONLY);
    if (fd == -1) {
        perror("Error opening file");
        return 1;
    }
    
    /* Get file size */
    struct stat st;
    if (fstat(fd, &st) == -1) {
        perror("Error getting file size");
        close(fd);
        return 1;
    }
    
    size_t file_size = st.st_size;
    if (file_size == 0) {
        printf("File is empty\n");
        close(fd);
        return 0;
    }
    
    /* Memory-map the entire file */
    char *data = mmap(NULL, file_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (data == MAP_FAILED) {
        perror("Error mapping file");
        close(fd);
        return 1;
    }
    
    /* Find and display first line */
    char *first_line = find_first_line(data, file_size);
    if (first_line) {
        printf("First line: %s\n", first_line);
        free(first_line);
    } else {
        printf("Could not read first line\n");
    }
    
    /* Find and display last line */
    char *last_line = find_last_line(data, file_size);
    if (last_line) {
        printf("Last line: %s\n", last_line);
        free(last_line);
    } else {
        printf("Could not read last line\n");
    }
    
    /* Clean up */
    if (munmap(data, file_size) == -1) {
        perror("Error unmapping file");
    }
    
    close(fd);
    return 0;
}
