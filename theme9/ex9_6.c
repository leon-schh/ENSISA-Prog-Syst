/*
 * Exercise 9.6: Proper signal handling practices
 * Same as 9.5 but applies good signal handling practices
 * (minimum operations in signal handlers)
 */
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>
#include <string.h>

static volatile sig_atomic_t counter = 0;
static volatile sig_atomic_t should_save = 0;
static volatile sig_atomic_t should_exit = 0;
static const char *filename = "counter_data.txt";

void sigint_handler(int sig) {
    (void)sig;
    /* Just set a flag - minimal operations in signal handler */
    should_save = 1;
}

void sigterm_handler(int sig) {
    (void)sig;
    /* Just set a flag - minimal operations in signal handler */
    should_exit = 1;
}

void save_counter_data(void) {
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
        printf("SIGINT saved to file. Counter: %ld\n", counter);
    }
}

void write_fin_and_exit(void) {
    FILE *fp;
    
    /* Write "fin" to file */
    fp = fopen(filename, "a");
    if (fp != NULL) {
        fprintf(fp, "fin\n");
        fclose(fp);
    }
    
    printf("SIGTERM processed, 'fin' written to file\n");
}

int main(void) {
    FILE *fp;
    
    /* Set up signal handlers */
    signal(SIGINT, sigint_handler);
    signal(SIGTERM, sigterm_handler);
    
    /* Initialize file */
    fp = fopen(filename, "w");
    if (fp != NULL) {
        fprintf(fp, "# Counter data file (proper signal handling)\n");
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
        
        /* Check flags set by signal handlers */
        if (should_save) {
            save_counter_data();
            should_save = 0;
        }
        
        /* Small delay to make counter increment visible */
        usleep(100000); /* 0.1 seconds */
    }
    
    /* Final cleanup */
    write_fin_and_exit();
    
    printf("Program terminated. Final counter value: %ld\n", counter);
    return 0;
}