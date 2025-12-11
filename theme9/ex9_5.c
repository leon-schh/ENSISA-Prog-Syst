/*
 * Exercise 9.5: Counter with SIGINT/SIGTERM file writing
 * Infinite loop with counter, saves data on SIGINT, terminates on SIGTERM
 * Groups file access operations in signal handlers (contrary to good practices)
 */
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>
#include <string.h>

static volatile sig_atomic_t counter = 0;
static volatile sig_atomic_t should_exit = 0;
static const char *filename = "counter_data.txt";

void sigint_handler(int sig) {
    (void)sig;
    FILE *fp;
    time_t now;
    struct tm *tm_info;
    char time_str[64];
    
    /* Get current time */
    time(&now);
    tm_info = localtime(&now);
    strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", tm_info);
    
    /* Open file in append mode and write data */
    fp = fopen(filename, "a");
    if (fp != NULL) {
        fprintf(fp, "SIGINT: %s, Counter: %ld\n", time_str, counter);
        fclose(fp);
    }
    
    printf("SIGINT received and saved to file. Counter: %ld\n", counter);
}

void sigterm_handler(int sig) {
    (void)sig;
    FILE *fp;
    
    /* Write "fin" to file and set exit flag */
    fp = fopen(filename, "a");
    if (fp != NULL) {
        fprintf(fp, "fin\n");
        fclose(fp);
    }
    
    printf("SIGTERM received, writing 'fin' to file and exiting\n");
    should_exit = 1;
}

int main(void) {
    FILE *fp;
    
    /* Set up signal handlers */
    signal(SIGINT, sigint_handler);
    signal(SIGTERM, sigterm_handler);
    
    /* Initialize file */
    fp = fopen(filename, "w");
    if (fp != NULL) {
        fprintf(fp, "# Counter data file\n");
        fclose(fp);
    }
    
    printf("Program started. PID: %d\n", getpid());
    printf("Press Ctrl+C (SIGINT) to save counter data\n");
    printf("Send SIGTERM to terminate and write 'fin' to file\n");
    printf("Example commands:\n");
    printf("  kill -SIGINT %d    (or press Ctrl+C)\n", getpid());
    printf("  kill -SIGTERM %d   \n", getpid());
    
    /* Main infinite loop */
    while (!should_exit) {
        counter++;
        /* Small delay to make counter increment visible */
        usleep(100000); /* 0.1 seconds */
    }
    
    printf("Program terminated. Final counter value: %ld\n", counter);
    return 0;
}