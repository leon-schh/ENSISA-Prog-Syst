/*
 * Exercice 5.4
 * Write a program to copy a file. Your program must take exactly two arguments:
 * the path of the existing file, and the path of the file (which may or may not exist)
 * that will contain the copy at the end of execution.
 * 
 * Verify (using a large file) that the copy was successful:
 * the cmp command called with both paths should not display anything.
 */

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

#define BUFFER_SIZE 8192

void copy_file(const char *src_path, const char *dst_path) {
    int fd_src, fd_dst;
    char buffer[BUFFER_SIZE];
    ssize_t bytes_read, bytes_written;
    struct stat st;
    
    /* Get source file info */
    if (stat(src_path, &st) == -1) {
        perror("Error getting source file info");
        return;
    }
    
    printf("Source file: %s (size: %ld bytes)\n", src_path, (long)st.st_size);
    
    /* Open source file for reading */
    fd_src = open(src_path, O_RDONLY);
    if (fd_src == -1) {
        perror("Error opening source file");
        return;
    }
    
    /* Create destination file for writing */
    fd_dst = open(dst_path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd_dst == -1) {
        perror("Error creating destination file");
        close(fd_src);
        return;
    }
    
    /* Copy file contents */
    while ((bytes_read = read(fd_src, buffer, BUFFER_SIZE)) > 0) {
        bytes_written = write(fd_dst, buffer, bytes_read);
        if (bytes_written != bytes_read) {
            perror("Error writing to destination file");
            close(fd_src);
            close(fd_dst);
            return;
        }
    }
    
    if (bytes_read == -1) {
        perror("Error reading from source file");
        close(fd_src);
        close(fd_dst);
        return;
    }
    
    /* Close files */
    close(fd_src);
    close(fd_dst);
    
    /* Get destination file info */
    if (stat(dst_path, &st) == -1) {
        perror("Error getting destination file info");
        return;
    }
    
    printf("Destination file: %s (size: %ld bytes)\n", dst_path, (long)st.st_size);
    printf("File copied successfully!\n");
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <source_file> <destination_file>\n", argv[0]);
        fprintf(stderr, "This program copies a file using system primitives.\n");
        return 1;
    }
    
    printf("Copying file: %s -> %s\n", argv[1], argv[2]);
    copy_file(argv[1], argv[2]);
    
    return 0;
}