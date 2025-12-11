/*
 * Exercise 9.13: Tube simulation with signals
 * Simulates pipe communication using signals
 */
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <errno.h>

#define READY SIGUSR1    /* Ready to communicate */
#define DATA_AVAILABLE SIGUSR2  /* Data available */
#define REQUEST_READ 1    /* Request to read */
#define REQUEST_WRITE 2   /* Request to write */

typedef struct {
    char buffer[1024];
    int length;
    int available;
    pid_t other_process;
    int direction;  /* 1 = read, 2 = write */
} tube_context;

static tube_context tube_data;
static volatile sig_atomic_t signal_received = 0;
static volatile sig_atomic_t current_signal = 0;

void signal_handler(int sig) {
    current_signal = sig;
    signal_received = 1;
}

/* Initialize tube structures */
void preparer_tube(void) {
    memset(&tube_data, 0, sizeof(tube_data));
    tube_data.available = 0;
    
    signal(SIGUSR1, signal_handler);
    signal(SIGUSR2, signal_handler);
    
    printf("Tube prepared\n");
}

/* Initialize access to tube for reading (sens=1) or writing (sens=2) */
void processus_tube(pid_t autre, int sens) {
    tube_data.other_process = autre;
    tube_data.direction = sens;
    
    printf("Tube process initialized: PID=%d, direction=%s\n", 
           autre, sens == 1 ? "READ" : "WRITE");
    
    /* Signal readiness to other process */
    kill(autre, READY);
}

/* Close tube */
void fermer_tube(void) {
    printf("Closing tube\n");
    memset(&tube_data, 0, sizeof(tube_data));
}

/* Write data to tube */
void ecrire_tube(void *buffer, int longueur) {
    if (tube_data.direction != 2) {
        fprintf(stderr, "Error: Not initialized for writing\n");
        return;
    }
    
    printf("Writing %d bytes to tube\n", longueur);
    
    if (longueur > sizeof(tube_data.buffer)) {
        fprintf(stderr, "Error: Data too large for tube\n");
        return;
    }
    
    /* Copy data to tube buffer */
    memcpy(tube_data.buffer, buffer, longueur);
    tube_data.length = longueur;
    tube_data.available = 1;
    
    /* Signal data availability */
    kill(tube_data.other_process, DATA_AVAILABLE);
    
    printf("Data written and signaled\n");
}

/* Read data from tube */
int lire_tube(void *buffer, int longueur) {
    if (tube_data.direction != 1) {
        fprintf(stderr, "Error: Not initialized for reading\n");
        return -1;
    }
    
    printf("Reading from tube (max %d bytes)\n", longueur);
    
    /* Wait for data availability */
    signal_received = 0;
    while (!signal_received) {
        pause();
    }
    
    if (current_signal != DATA_AVAILABLE) {
        fprintf(stderr, "Error: Unexpected signal %d\n", current_signal);
        return -1;
    }
    
    /* Copy data from tube buffer */
    int to_read = (longueur < tube_data.length) ? longueur : tube_data.length;
    memcpy(buffer, tube_data.buffer, to_read);
    
    printf("Read %d bytes from tube\n", to_read);
    
    /* Signal readiness for next operation */
    kill(tube_data.other_process, READY);
    
    return to_read;
}

/* Child process - reader */
void child_reader(void) {
    char read_buffer[1024];
    int bytes_read;
    
    printf("Child (reader) process started, PID: %d\n", getpid());
    
    preparer_tube();
    processus_tube(getppid(), 1);  /* READ direction */
    
    /* Wait for parent to be ready */
    signal_received = 0;
    while (!signal_received) {
        pause();
    }
    
    /* Read data */
    bytes_read = lire_tube(read_buffer, sizeof(read_buffer));
    
    if (bytes_read > 0) {
        printf("Child received: '%.*s'\n", bytes_read, read_buffer);
    }
    
    fermer_tube();
    exit(0);
}

/* Parent process - writer */
void parent_writer(pid_t child_pid) {
    const char *message = "Hello from parent via tube!";
    
    printf("Parent (writer) process started, PID: %d\n", getpid());
    
    preparer_tube();
    processus_tube(child_pid, 2);  /* WRITE direction */
    
    /* Wait for child to be ready */
    signal_received = 0;
    while (!signal_received) {
        pause();
    }
    
    /* Write data */
    ecrire_tube((void *)message, strlen(message));
    
    /* Wait for child to finish */
    int status;
    wait(&status);
    
    printf("Parent: Child finished\n");
    fermer_tube();
}

int main(void) {
    pid_t pid;
    
    printf("Tube simulation with signals\n");
    printf("Using signals for pipe-like communication:\n");
    printf("  READY = SIGUSR1 (ready to communicate)\n");
    printf("  DATA_AVAILABLE = SIGUSR2 (data available)\n");
    
    pid = fork();
    
    if (pid == -1) {
        perror("fork");
        exit(1);
    }
    
    if (pid == 0) {
        /* Child process */
        child_reader();
    } else {
        /* Parent process */
        parent_writer(pid);
    }
    
    return 0;
}