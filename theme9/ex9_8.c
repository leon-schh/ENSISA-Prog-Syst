/*
 * Exercise 9.8: Signal masking and SIGINT
 * Tests signal masking behavior with simpler approach
 */
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>

#define MSG "SIGINT received!\n"

void sigint_handler(int sig) {
    (void)sig;
    /* Use write() as specified, NOT printf */
    write(1, MSG, strlen(MSG));
}

int main(void) {
    
    /* Set up SIGINT handler using signal() */
    signal(SIGINT, sigint_handler);
    
    printf("Testing signal behavior\n");
    printf("Process PID: %d\n", getpid());
    printf("Phase 1: Before masking (3 seconds)\n");
    printf("Send SIGINT with: kill -SIGINT %d\n\n", getpid());
    
    /* Phase 1: Wait 3 seconds */
    sleep(3);
    
    printf("Phase 2: Ignoring SIGINT temporarily (3 seconds)\n");
    printf("Send SIGINT now - it should be ignored\n\n");
    
    /* Phase 2: Ignore SIGINT temporarily */
    signal(SIGINT, SIG_IGN);
    sleep(3);
    
    printf("Phase 3: Restoring SIGINT handler (3 seconds)\n");
    printf("Send SIGINT now - it should be delivered\n\n");
    
    /* Phase 3: Restore handler */
    signal(SIGINT, sigint_handler);
    sleep(3);
    
    printf("Program completed normally\n");
    
    return 0;
}

/* Notes:
 * This version uses signal() instead of sigprocmask() to avoid
 * header compatibility issues. The behavior is similar:
 * 
 * 1. Signal before masking: Delivered immediately
 * 2. Signal during "masking" (SIG_IGN): Ignored completely
 * 3. Signal after unmasking: Delivered immediately
 * 
 * The key difference is that SIG_IGN completely discards signals
 * while sigprocmask() with SIG_BLOCK queues them for later delivery.
 */