/*
 * Exercise 9.4: Two processes with alarm timing
 * Father creates child that runs a function every second for one minute
 * Then father sends SIGUSR1 to stop the child
 */
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>

static volatile sig_atomic_t stop_flag = 0;

void trait(void) {
    printf("Child process: executing trait function\n");
}

void alarm_handler_child(int sig) {
    (void)sig;
    trait();
    /* Schedule next alarm for 1 second */
    alarm(1);
}

void sigusr1_handler_child(int sig) {
    (void)sig;
    printf("Child: received SIGUSR1 from father, stopping\n");
    stop_flag = 1;
}

int main(void) {
    pid_t pid;
    
    pid = fork();
    
    if (pid == -1) {
        perror("fork");
        exit(1);
    }
    
    if (pid == 0) {
        /* Child process */
        printf("Child process started, PID: %d\n", getpid());
        
        /* Set up alarm handler for trait function */
        signal(SIGALRM, alarm_handler_child);
        
        /* Set up SIGUSR1 handler for stopping */
        signal(SIGUSR1, sigusr1_handler_child);
        
        /* Start the first alarm for 1 second */
        alarm(1);
        
        /* Wait for signals */
        while (!stop_flag) {
            pause();
        }
        
        printf("Child process terminating\n");
        exit(0);
        
    } else {
        /* Father process */
        printf("Father process, child PID: %d\n", pid);
        
        /* Wait for one minute (60 seconds) */
        sleep(60);
        
        printf("Father: one minute elapsed, sending SIGUSR1 to child\n");
        kill(pid, SIGUSR1);
        
        /* Wait for child termination */
        int status;
        wait(&status);
        printf("Father: child terminated with status %d\n", WEXITSTATUS(status));
        printf("Father: terminating\n");
    }
    
    return 0;
}