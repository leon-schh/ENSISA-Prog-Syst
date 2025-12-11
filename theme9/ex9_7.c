/*
 * Exercise 9.7: SIGUSR1 handler with 1 million signals
 * Tests signal handling reliability and race conditions
 */
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>

static volatile sig_atomic_t sigusr1_counter = 0;
static volatile sig_atomic_t decrement_counter = 0;

void sigusr1_handler(int sig) {
    (void)sig;
    sigusr1_counter++;
    decrement_counter--;
}

int main(void) {
    pid_t pid;
    int status;
    
    /* Set up SIGUSR1 handler */
    signal(SIGUSR1, sigusr1_handler);
    
    printf("Father process PID: %d\n", getpid());
    
    pid = fork();
    
    if (pid == -1) {
        perror("fork");
        exit(1);
    }
    
    if (pid == 0) {
        /* Child process */
        printf("Child process started, PID: %d\n", getpid());
        printf("Sending 1 million SIGUSR1 signals to father...\n");
        
        for (int i = 0; i < 1000000; i++) {
            kill(getppid(), SIGUSR1);
        }
        
        printf("Child: sent all signals, terminating\n");
        exit(0);
        
    } else {
        /* Father process */
        printf("Father: waiting for child to send signals and terminate\n");
        
        /* Initialize decrement counter */
        decrement_counter = 1000000;
        
        /* Decrement counter 1 million times while waiting for child */
        for (int i = 0; i < 1000000; i++) {
            decrement_counter--;
            
            /* Wait for child with retry on EINTR */
            if (waitpid(pid, &status, WNOHANG) > 0) {
                break;
            }
        }
        
        /* Final wait with retry on EINTR */
        do {
            status = wait(&status);
        } while (status == -1 && errno == EINTR);
        
        printf("Father: child terminated\n");
        printf("Father: SIGUSR1 counter = %d (expected 1000000)\n", sigusr1_counter);
        printf("Father: decrement counter = %d (expected 0)\n", decrement_counter);
        
        if (sigusr1_counter == 1000000) {
            printf("SUCCESS: All signals received!\n");
        } else {
            printf("WARNING: Some signals lost! Lost: %d\n", 1000000 - sigusr1_counter);
        }
        
        if (decrement_counter == 0) {
            printf("SUCCESS: All decrements completed correctly!\n");
        } else {
            printf("WARNING: Race condition detected! Counter = %d\n", decrement_counter);
        }
    }
    
    return 0;
}