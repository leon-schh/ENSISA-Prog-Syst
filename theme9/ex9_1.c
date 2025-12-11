/*
 * Exercise 9.1: Counter increment on SIGINT
 * Increments and displays a counter each time SIGINT is received.
 * Stops after 5 times. Increment/display NOT in main.
 */
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

static volatile int counter = 0;

void sigint_handler(int sig) {
    (void)sig;
    counter++;
    printf("Counter: %d\n", counter);
    if (counter >= 5) {
        printf("Received SIGINT 5 times, exiting.\n");
        exit(0);
    }
}

int main(void) {
    struct sigaction sa;
    sa.sa_handler = sigint_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    
    if (sigaction(SIGINT, &sa, NULL) == -1) {
        perror("sigaction");
        exit(1);
    }
    
    printf("Press Ctrl+C to increment counter (5 times to exit)\n");
    
    while (1) {
        pause();
    }
    
    return 0;
}
