/*
 * Exercise 9.12: Bit transmission simulation
 * Simulates serial communication using SIGUSR1/SIGUSR2 signals
 * 0 = SIGUSR1, 1 = SIGUSR2, ack = SIGUSR1
 */
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

#define BIT_0 SIGUSR1  /* 0 bit */
#define BIT_1 SIGUSR2  /* 1 bit */
#define ACK SIGUSR1    /* Acknowledgment */

static volatile sig_atomic_t current_bit = -1;
static volatile sig_atomic_t bit_received = 0;
static volatile sig_atomic_t ack_received = 0;

void bit_handler(int sig) {
    current_bit = (sig == SIGUSR1) ? 0 : 1;
    bit_received = 1;
}

void ack_handler(int sig) {
    (void)sig;
    ack_received = 1;
}

/* Prepare receiver to receive an octet */
void preparer_reception(void) {
    signal(SIGUSR1, bit_handler);
    signal(SIGUSR2, bit_handler);
    current_bit = -1;
    bit_received = 0;
}

/* Send 8 bits of an octet to receiver */
void envoyer(pid_t recepteur, int octet) {
    printf("Sending byte: %d (0x%02x)\n", octet, octet);
    
    for (int i = 0; i < 8; i++) {
        int bit = (octet >> i) & 1;  /* Extract bit i (LSB first) */
        
        /* Send bit */
        if (bit == 0) {
            printf("  Sending bit 0 (SIGUSR1)\n");
            kill(recepteur, BIT_0);
        } else {
            printf("  Sending bit 1 (SIGUSR2)\n");
            kill(recepteur, BIT_1);
        }
        
        /* Wait for acknowledgment */
        ack_received = 0;
        while (!ack_received) {
            pause();
        }
        printf("  Acknowledgment received\n");
    }
    
    printf("Byte transmission completed\n");
}

/* Wait to receive 8 bits and reconstruct octet */
int recevoir(pid_t emetteur) {
    int octet = 0;
    
    printf("Waiting to receive byte...\n");
    
    for (int i = 0; i < 8; i++) {
        /* Wait for bit */
        bit_received = 0;
        while (!bit_received) {
            pause();
        }
        
        int bit = current_bit;
        printf("  Received bit %d\n", bit);
        
        /* Reconstruct octet (LSB first) */
        octet |= (bit << i);
        
        /* Send acknowledgment */
        kill(emetteur, ACK);
        printf("  Acknowledgment sent\n");
    }
    
    printf("Byte received: %d (0x%02x)\n", octet, octet);
    return octet;
}

/* Child process (receiver) */
void child_process(void) {
    int received_byte;
    
    printf("Child process (receiver) started, PID: %d\n", getpid());
    
    /* Set up acknowledgment handler */
    signal(SIGUSR1, ack_handler);
    
    /* Prepare to receive */
    preparer_reception();
    
    /* Receive byte from parent */
    received_byte = recevoir(getppid());
    
    printf("Child: Successfully received byte %d\n", received_byte);
    exit(received_byte);
}

/* Parent process (sender) */
void parent_process(pid_t child_pid) {
    int byte_to_send = 42;  /* Example byte value */
    
    printf("Parent process (sender), PID: %d\n", getpid());
    printf("Sending byte value: %d\n", byte_to_send);
    
    /* Set up bit handlers */
    signal(SIGUSR1, bit_handler);
    signal(SIGUSR2, bit_handler);
    
    /* Send byte to child */
    envoyer(child_pid, byte_to_send);
    
    /* Wait for child to finish */
    int status;
    wait(&status);
    
    if (WIFEXITED(status)) {
        printf("Parent: Child terminated with exit code %d\n", WEXITSTATUS(status));
    }
}

int main(void) {
    pid_t pid;
    
    printf("Bit transmission simulation\n");
    printf("Using signals for serial communication:\n");
    printf("  0 bit = SIGUSR1\n");
    printf("  1 bit = SIGUSR2\n");
    printf("  acknowledgment = SIGUSR1\n");
    
    pid = fork();
    
    if (pid == -1) {
        perror("fork");
        exit(1);
    }
    
    if (pid == 0) {
        /* Child process */
        child_process();
    } else {
        /* Parent process */
        parent_process(pid);
    }
    
    return 0;
}