#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>

#define MANAGER_FIFO "gestionnaire_T"
#define BUFFER_SIZE 1024

// Message types (must match manager)
#define MSG_SUBSCRIBE 1
#define MSG_SEND 2
#define MSG_LIST 3
#define MSG_UNSUBSCRIBE 4
#define MSG_ACK 5

// Message structure (must match manager)
struct message {
    int type;
    int sender_id;
    int receiver_id;
    int length;
    char data[BUFFER_SIZE];
};

// Communication structure
struct comm {
    int manager_fd;        // File descriptor for manager pipe
    int client_fd;         // File descriptor for client pipe
    char fifo_name[256];   // Client's FIFO name
    int client_id;         // Assigned by manager
    int subscribed;        // 1 if subscribed to manager
};

/**
 * Communication type definition for exercise 8.8
 */
typedef struct comm *comm;

/**
 * Initialize communication with manager
 */
comm initialiser(char tube[]) {
    comm c = malloc(sizeof(struct comm));
    if (!c) {
        return NULL;
    }
    
    // Generate unique FIFO name for this client
    snprintf(c->fifo_name, sizeof(c->fifo_name), "client_%d", getpid());
    
    // Create client FIFO
    unlink(c->fifo_name);
    if (mkfifo(c->fifo_name, 0666) == -1) {
        perror("mkfifo client");
        free(c);
        return NULL;
    }
    
    // Open manager FIFO for writing
    c->manager_fd = open(MANAGER_FIFO, O_WRONLY);
    if (c->manager_fd == -1) {
        perror("open manager fifo");
        unlink(c->fifo_name);
        free(c);
        return NULL;
    }
    
    // Subscribe to manager
    struct message msg;
    msg.type = MSG_SUBSCRIBE;
    msg.sender_id = getpid();
    msg.receiver_id = 0;
    msg.length = strlen(c->fifo_name) + 1;
    strcpy(msg.data, c->fifo_name);
    
    if (write(c->manager_fd, &msg, sizeof(struct message)) != sizeof(struct message)) {
        perror("write subscribe");
        close(c->manager_fd);
        unlink(c->fifo_name);
        free(c);
        return NULL;
    }
    
    // Open client FIFO for reading
    c->client_fd = open(c->fifo_name, O_RDONLY);
    if (c->client_fd == -1) {
        perror("open client fifo");
        close(c->manager_fd);
        unlink(c->fifo_name);
        free(c);
        return NULL;
    }
    
    c->subscribed = 1;
    c->client_id = -1;
    
    return c;
}

/**
 * Send message to manager
 */
int send_to_manager(comm c, struct message *msg) {
    if (!c || !c->subscribed) {
        return -1;
    }
    
    msg->sender_id = getpid();
    
    ssize_t written = write(c->manager_fd, msg, sizeof(struct message));
    return (written == sizeof(struct message)) ? 0 : -1;
}

/**
 * Get list of subscribed clients
 */
int liste(comm gerant, int abonnes[], int maxab) {
    if (!gerant || !gerant->subscribed) {
        return -1;
    }
    
    // Send list request to manager
    struct message msg;
    msg.type = MSG_LIST;
    msg.receiver_id = 0;
    msg.length = 0;
    
    if (send_to_manager(gerant, &msg) == -1) {
        return -1;
    }
    
    // Wait for response
    struct message response;
    ssize_t bytes_read = read(gerant->client_fd, &response, sizeof(struct message));
    
    if (bytes_read != sizeof(struct message) || response.type != MSG_LIST) {
        return -1;
    }
    
    // Extract client PIDs
    int num_clients = response.length / sizeof(int);
    if (num_clients > maxab) {
        num_clients = maxab;
    }
    
    for (int i = 0; i < num_clients; i++) {
        memcpy(&abonnes[i], response.data + i * sizeof(int), sizeof(int));
    }
    
    return num_clients;
}

/**
 * Send message to another client
 */
int envoyer(comm gerant, int abonne, char *message, int lg) {
    if (!gerant || !gerant->subscribed || !message) {
        return -1;
    }
    
    struct message msg;
    msg.type = MSG_SEND;
    msg.receiver_id = abonne;
    msg.length = lg;
    
    if (lg > BUFFER_SIZE) {
        return -1;
    }
    
    memcpy(msg.data, message, lg);
    
    return send_to_manager(gerant, &msg);
}

/**
 * Receive message from another client
 */
int recevoir(comm c, char *message, int *lgmax) {
    if (!c || !message || !lgmax) {
        return -1;
    }
    
    struct message msg;
    ssize_t bytes_read = read(c->client_fd, &msg, sizeof(struct message));
    
    if (bytes_read != sizeof(struct message)) {
        return -1;
    }
    
    if (msg.type == MSG_SEND) {
        int copy_length = (msg.length < *lgmax) ? msg.length : *lgmax;
        memcpy(message, msg.data, copy_length);
        *lgmax = msg.length;
        return msg.sender_id;
    }
    
    return -1;
}

/**
 * Simple test application: ask other clients for their process numbers and display minimum
 */
void test_application(comm c) {
    printf("Client PID: %d\n", getpid());
    
    // Get list of all subscribed clients
    int abonnes[64];
    int num_abonnes = liste(c, abonnes, 64);
    
    if (num_abonnes < 0) {
        fprintf(stderr, "Erreur lors de la récupération de la liste\n");
        return;
    }
    
    printf("Nombre d'abonnés: %d\n", num_abonnes);
    
    // Find minimum PID among all clients (including ourselves)
    int min_pid = getpid();
    
    for (int i = 0; i < num_abonnes; i++) {
        printf("Abonné %d: PID %d\n", i, abonnes[i]);
        if (abonnes[i] < min_pid) {
            min_pid = abonnes[i];
        }
    }
    
    printf("PID minimum parmi tous les clients: %d\n", min_pid);
    
    // Send a test message to each client
    char test_msg[] = "Bonjour du client";
    for (int i = 0; i < num_abonnes; i++) {
        if (abonnes[i] != getpid()) { // Don't send to ourselves
            printf("Envoi de message au client PID %d\n", abonnes[i]);
            envoyer(c, i, test_msg, strlen(test_msg) + 1);
        }
    }
    
    // Receive messages
    printf("Attente de messages...\n");
    for (int i = 0; i < num_abonnes - 1; i++) {
        char buffer[BUFFER_SIZE];
        int lgmax = BUFFER_SIZE;
        int sender = recevoir(c, buffer, &lgmax);
        
        if (sender >= 0) {
            printf("Message reçu de PID %d: %s\n", sender, buffer);
        }
    }
}

/**
 * Exercise 8.8: Client processes for communication system
 */
int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <manager_fifo>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    
    // Use provided FIFO name
    strcpy(MANAGER_FIFO, argv[1]);
    
    printf("=== Client de Communication ===\n");
    printf("PID: %d\n", getpid());
    
    // Initialize communication
    comm c = initialiser(MANAGER_FIFO);
    if (!c) {
        fprintf(stderr, "Erreur d'initialisation\n");
        exit(EXIT_FAILURE);
    }
    
    printf("Communication initialisée\n");
    
    // Run test application
    test_application(c);
    
    printf("Test terminé\n");
    
    // Cleanup
    close(c->manager_fd);
    close(c->client_fd);
    unlink(c->fifo_name);
    free(c);
    
    return 0;
}