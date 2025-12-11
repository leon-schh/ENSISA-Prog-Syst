/*
 * Exercise 9.10: Command launching with SIGALRM
 * Launches a command every second, avoids zombie processes with SIGCHLD
 */
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

static volatile sig_atomic_t should_launch = 0;

void alarm_handler(int sig) {
    (void)sig;
    should_launch = 1;
}

void sigchld_handler(int sig) {
    (void)sig;
    /* Reap child processes to avoid zombies */
    while (waitpid(-1, NULL, WNOHANG) > 0) {
        /* Continue reaping until no more children */
    }
}

void launch_command(void) {
    pid_t pid;
    
    pid = fork();
    
    if (pid == -1) {
        perror("fork");
        return;
    }
    
    if (pid == 0) {
        /* Child process - execute command */
        /* For this example, use 'date' command */
        execlp("date", "date", "+%H:%M:%S", (char *)NULL);
        
        /* If execlp fails */
        perror("execlp");
        exit(1);
        
    } else {
        /* Parent process */
        printf("Launched command at %ld\n", (long)time(NULL));
        /* Child will be reaped by SIGCHLD handler */
    }
}

int main(void) {
    const char *command = "date";
    
    /* Set up signal handlers */
    signal(SIGALRM, alarm_handler);
    signal(SIGCHLD, sigchld_handler);
    
    printf("Command launcher started\n");
    printf("Process PID: %d\n", getpid());
    printf("Command: %s\n", command);
    printf("Will launch every 1 second\n");
    printf("Press Ctrl+C to stop\n");
    
    /* Schedule first alarm */
    alarm(1);
    
    while (1) {
        /* Wait for signal */
        pause();
        
        /* Check if we should launch a command */
        if (should_launch) {
            launch_command();
            should_launch = 0;
            
            /* Schedule next alarm */
            alarm(1);
        }
    }
    
    return 0;
}