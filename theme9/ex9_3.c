/*
 * Exercise 9.3: Signal meaning display
 * Waits for any signal and displays its meaning
 */
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

void signal_handler(int sig) {
    /* Use psignal to display the meaning of the signal */
    psignal(sig, "Received signal");
    printf("Signal number: %d\n", sig);
    exit(0);
}

int main(void) {
    /* Simple approach: set up handlers for common signals individually */
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);
    signal(SIGUSR1, signal_handler);
    signal(SIGUSR2, signal_handler);
    signal(SIGILL, signal_handler);
    signal(SIGABRT, signal_handler);
    signal(SIGFPE, signal_handler);
    signal(SIGSEGV, signal_handler);
    
    printf("Waiting for any signal...\n");
    printf("Try sending different signals, e.g.:\n");
    printf("  kill -SIGINT %d    (or press Ctrl+C)\n", getpid());
    printf("  kill -SIGTERM %d   \n", getpid());
    printf("  kill -SIGUSR1 %d   \n", getpid());
    printf("  kill -SIGILL %d    \n", getpid());
    
    while (1) {
        pause();  /* Wait for any signal */
    }
    
    return 0;
}