/*
 * Exercice 5.3
 * Write a program that copies file "toto" to file "titi" using system primitives.
 * Do not try to create the new file with the permissions of the original file.
 */

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>

#define BUFFER_SIZE 4096

int main(void) {
    int fd_src, fd_dst;
    char buffer[BUFFER_SIZE];
    ssize_t bytes_read, bytes_written;
    
    /* Open source file for reading */
    fd_src = open("toto", O_RDONLY);
    if (fd_src == -1) {
        perror("Error opening source file toto");
        return 1;
    }
    
    /* Create destination file for writing (overwrite if exists) */
    fd_dst = open("titi", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd_dst == -1) {
        perror("Error creating destination file titi");
        close(fd_src);
        return 1;
    }
    
    /* Copy file contents */
    while ((bytes_read = read(fd_src, buffer, BUFFER_SIZE)) > 0) {
        bytes_written = write(fd_dst, buffer, bytes_read);
        if (bytes_written != bytes_read) {
            perror("Error writing to destination file");
            close(fd_src);
            close(fd_dst);
            return 1;
        }
    }
    
    if (bytes_read == -1) {
        perror("Error reading from source file");
        close(fd_src);
        close(fd_dst);
        return 1;
    }
    
    /* Close files */
    if (close(fd_src) == -1) {
        perror("Error closing source file");
    }
    
    if (close(fd_dst) == -1) {
        perror("Error closing destination file");
    }
    
    printf("File copied successfully from toto to titi\n");
    return 0;
}