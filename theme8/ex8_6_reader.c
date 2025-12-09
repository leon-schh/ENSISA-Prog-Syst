#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>

#define BUFFER_SIZE 4096
#define FIFO_NAME "mon_tube_nomme"

/**
 * Copy data from source file descriptor to destination file descriptor
 */
void copier(int fdsrc, int fddst) {
    char buffer[BUFFER_SIZE];
    ssize_t bytes_read;
    
    while ((bytes_read = read(fdsrc, buffer, BUFFER_SIZE)) > 0) {
        ssize_t bytes_written = 0;
        ssize_t total_written = 0;
        
        while (total_written < bytes_read) {
            bytes_written = write(fddst, buffer + total_written, bytes_read - total_written);
            if (bytes_written < 0) {
                perror("Erreur lors de l'écriture");
                return;
            }
            total_written += bytes_written;
        }
    }
    
    if (bytes_read < 0) {
        perror("Erreur lors de la lecture");
    }
}

/**
 * Exercise 8.6 - Reader part: Opens named pipe and reads data to stdout
 */
int main(void) {
    int fd;
    
    printf("Tentative d'ouverture du tube nommé '%s'...\n", FIFO_NAME);
    
    // Open named pipe for reading
    fd = open(FIFO_NAME, O_RDONLY);
    if (fd == -1) {
        perror("open");
        fprintf(stderr, "Le writer doit être lancé en premier\n");
        exit(EXIT_FAILURE);
    }
    
    printf("Tube ouvert en lecture. Affichage des données reçues...\n");
    
    // Read from named pipe and write to stdout
    copier(fd, STDOUT_FILENO);
    
    printf("\nTransfert terminé.\n");
    
    close(fd);
    
    return 0;
}