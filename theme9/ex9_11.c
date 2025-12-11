/*
 * Exercise 9.11: Nested signal handling
 * Displays nested signal processing levels with tabs
 */
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>

static volatile sig_atomic_t current_level = 0;
static volatile sig_atomic_t processing_signal = 0;

void sigint_handler(int sig) {
    (void)sig;
    
    if (!processing_signal) {
        /* First level signal */
        processing_signal = 1;
        current_level = 1;
        process_signal_level();
        processing_signal = 0;
    } else {
        /* Nested signal - increase level */
        current_level++;
        process_signal_level();
        current_level--;
    }
}

void process_signal_level(void) {
    for (int i = 0; i < 5; i++) {
        /* Display tabs corresponding to current level */
        for (int j = 0; j < current_level; j++) {
            printf("\t");
        }
        printf("traitement du signal de niveau %d (iteration %d)\n", current_level, i + 1);
        sleep(1);
    }
}

void normal_treatment(void) {
    printf("traitement normal\n");
}

int main(void) {
    
    /* Set up SIGINT handler */
    signal(SIGINT, sigint_handler);
    
    printf("Nested signal handling program\n");
    printf("Process PID: %d\n", getpid());
    printf("Press Ctrl+C (SIGINT) to start signal processing\n");
    printf("Press Ctrl+C again during processing to increase level\n");
    printf("Use SA_NODEFER behavior for signal nesting\n");
    
    while (1) {
        normal_treatment();
        sleep(1);
    }
    
    return 0;
}

/* Notes:
 * This demonstrates nested signal handling where:
 * 
 * 1. First SIGINT starts level 1 processing (5 iterations)
 * 2. Second SIGINT during processing increases to level 2
 * 3. Third SIGINT increases to level 3, etc.
 * 
 * The SA_NODEFER flag (in sigaction) would allow the signal
 * to be delivered even while the handler is executing, enabling
 * true nesting. With signal(), the behavior depends on the system.
 * 
 * Each level shows its processing with increasing indentation
 * to make the nesting visible.
 */