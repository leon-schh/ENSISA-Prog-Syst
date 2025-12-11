/*
 * Exercise 9.9: Alarm and sigsuspend
 * Simplified version testing alarm and signal handling
 */
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

static volatile sig_atomic_t received = 0;

void alarm_handler(int sig) {
    (void)sig;
    received = 1;
}

int main(void) {
    
    /* Set up alarm handler */
    signal(SIGALRM, alarm_handler);
    
    printf("Testing alarm and signal handling\n");
    printf("Process PID: %d\n", getpid());
    
    /* Test 1: Alarm after 2 seconds, wait 1 second before checking */
    printf("\n=== Test 1: Alarm in 2s, wait 1s, then check ===\n");
    received = 0;
    
    /* Request alarm in 2 seconds */
    alarm(2);
    printf("Alarm set for 2 seconds\n");
    
    /* Wait 1 second */
    printf("Sleeping for 1 second...\n");
    sleep(1);
    
    printf("Checking if signal received...\n");
    if (received) {
        printf("Signal already received during sleep\n");
    } else {
        printf("Signal not received yet, waiting...\n");
        /* Simple wait - in real implementation would use sigsuspend */
        while (!received) {
            pause();
        }
    }
    
    if (received) {
        printf("SUCCESS: Signal received\n");
    } else {
        printf("ERROR: Signal not received\n");
    }
    
    /* Cancel any remaining alarm */
    alarm(0);
    
    /* Test 2: Alarm after 1 second, wait 2 seconds before checking */
    printf("\n=== Test 2: Alarm in 1s, wait 2s, then check ===\n");
    received = 0;
    
    /* Request alarm in 1 second */
    alarm(1);
    printf("Alarm set for 1 second\n");
    
    /* Wait 2 seconds */
    printf("Sleeping for 2 seconds...\n");
    sleep(2);
    
    /* Check if signal was already received */
    if (received) {
        printf("SUCCESS: Signal received during sleep\n");
    } else {
        printf("ERROR: Signal not received during sleep\n");
        /* Wait if not received */
        while (!received) {
            pause();
        }
        printf("Signal eventually received via pause\n");
    }
    
    /* Test 3: Race condition demonstration */
    printf("\n=== Test 3: Demonstrating race condition ===\n");
    received = 0;
    
    /* Set alarm very quickly */
    alarm(1);
    printf("Alarm set for 1 second\n");
    
    /* Check immediately - this demonstrates the race condition */
    printf("Checking immediately (race condition)...\n");
    if (received) {
        printf("Signal received immediately (unlikely)\n");
    } else {
        printf("Signal not received immediately (expected)\n");
        printf("Now waiting properly...\n");
        while (!received) {
            pause();
        }
        printf("Signal received after proper wait\n");
    }
    
    printf("\nAll tests completed\n");
    return 0;
}

/* Notes:
 * This simplified version demonstrates the core concepts:
 * 
 * 1. Alarm scheduling and signal delivery
 * 2. Race conditions between signal arrival and checking
 * 3. Proper waiting with pause() instead of busy waiting
 * 
 * The original exercise uses sigsuspend() which provides atomic
 * signal masking during wait, preventing race conditions.
 */