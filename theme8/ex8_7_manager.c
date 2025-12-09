#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>
#include <signal.h>
#include <sys/select.h>

#define MANAGER_FIFO "gestionnaire_T"
#define MAX_CLIENTS 64
#define BUFFER_SIZE 1024
#define FIFO_PREFIX "T_"

// Message types
#define MSG_SUBSCRIBE 1    // Client subscribes
#define MSG_SEND 2         // Send message to another client
#define MSG_LIST 3         // Request client list
#define MSG_UNSUBSCRIBE 4  // Client unsubscribes
#define MSG_ACK 5          // Acknowledgment

// Message structure
struct message {
    int type;           // Message type
    int sender_id;      // Sender process ID (0 for manager)
    int receiver_id;    // Receiver process ID (0 for broadcast)
    int length;         // Message length
    char data[BUFFER_SIZE]; // Message data
};

// Client information
struct client {
    int pid;            // Client process ID
    int subscribed;     // 1 if subscribed, 0 otherwise
    char fifo_name[256]; // Client's named pipe
};

struct client clients[MAX_CLIENTS];
int num_clients = 0;

/**
 * Create manager pipe
 */
int create_manager_pipe() {
    unlink(MANAGER_FIFO);
    if (mkfifo(MANAGER_FIFO, 0666) == -1) {
        perror("mkfifo manager");
        return -1;
    }
    return 0;
}

/**
 * Add client to the list
 */
int add_client(int pid, const char *fifo_name) {
    if (num_clients >= MAX_CLIENTS) {
        return -1;
    }
    
    clients[num_clients].pid = pid;
    clients[num_clients].subscribed = 1;
    strcpy(clients[num_clients].fifo_name, fifo_name);
    num_clients++;
    
    printf("Client %d ajouté (pipe: %s)\n", pid, fifo_name);
    return num_clients - 1;
}

/**
 * Remove client from the list
 */
void remove_client(int pid) {
    for (int i = 0; i < num_clients; i++) {
        if (clients[i].pid == pid) {
            clients[i].subscribed = 0;
            printf("Client %d désabonné\n", pid);
            break;
        }
    }
}

/**
 * Send message to a client
 */
int send_to_client(int receiver_id, struct message *msg) {
    if (receiver_id < 0 || receiver_id >= num_clients || 
        !clients[receiver_id].subscribed) {
        return -1;
    }
    
    int fd = open(clients[receiver_id].fifo_name, O_WRONLY);
    if (fd == -1) {
        perror("open client fifo");
        return -1;
    }
    
    ssize_t written = write(fd, msg, sizeof(struct message));
    close(fd);
    
    return (written == sizeof(struct message)) ? 0 : -1;
}

/**
 * Send acknowledgment to sender
 */
void send_ack(int client_id, int success) {
    struct message ack_msg;
    ack_msg.type = MSG_ACK;
    ack_msg.sender_id = 0;
    ack_msg.receiver_id = client_id;
    ack_msg.length = sizeof(int);
    memcpy(ack_msg.data, &success, sizeof(int));
    
    send_to_client(client_id, &ack_msg);
}

/**
 * Handle subscribe message
 */
void handle_subscribe(struct message *msg) {
    char client_fifo[256];
    int client_pid = msg->sender_id;
    
    // Extract client FIFO name from message
    strcpy(client_fifo, msg->data);
    
    int client_id = add_client(client_pid, client_fifo);
    if (client_id >= 0) {
        send_ack(client_id, 1); // Success
        printf("Client %d (PID: %d) subscribed successfully\n", client_id, client_pid);
    } else {
        send_ack(num_clients, 0); // Failure
    }
}

/**
 * Handle send message
 */
void handle_send(struct message *msg) {
    int receiver_id = msg->receiver_id;
    
    if (receiver_id == 0) {
        // Broadcast to all clients
        for (int i = 0; i < num_clients; i++) {
            if (clients[i].subscribed) {
                msg->receiver_id = i;
                send_to_client(i, msg);
            }
        }
        msg->receiver_id = 0; // Reset
        send_ack(msg->sender_id, 1);
    } else {
        // Send to specific client
        if (send_to_client(receiver_id, msg) == 0) {
            send_ack(msg->sender_id, 1);
        } else {
            send_ack(msg->sender_id, 0);
        }
    }
}

/**
 * Handle list request
 */
void handle_list(struct message *msg) {
    struct message list_msg;
    list_msg.type = MSG_LIST;
    list_msg.sender_id = 0;
    list_msg.receiver_id = msg->sender_id;
    list_msg.length = num_clients * sizeof(int);
    
    // Copy client PIDs to message
    for (int i = 0; i < num_clients && i * sizeof(int) < BUFFER_SIZE; i++) {
        memcpy(list_msg.data + i * sizeof(int), &clients[i].pid, sizeof(int));
    }
    
    send_to_client(msg->sender_id, &list_msg);
    send_ack(msg->sender_id, 1);
}

/**
 * Handle unsubscribe message
 */
void handle_unsubscribe(struct message *msg) {
    remove_client(msg->sender_id);
    send_ack(msg->sender_id, 1);
}

/**
 * Process message based on type
 */
void process_message(struct message *msg) {
    printf("Message reçu: type=%d, sender=%d, receiver=%d\n", 
           msg->type, msg->sender_id, msg->receiver_id);
    
    switch (msg->type) {
        case MSG_SUBSCRIBE:
            handle_subscribe(msg);
            break;
        case MSG_SEND:
            handle_send(msg);
            break;
        case MSG_LIST:
            handle_list(msg);
            break;
        case MSG_UNSUBSCRIBE:
            handle_unsubscribe(msg);
            break;
        default:
            printf("Type de message inconnu: %d\n", msg->type);
    }
}

/**
 * Signal handler for clean termination
 */
void signal_handler(int sig) {
    unlink(MANAGER_FIFO);
    printf("\nGestionnaire terminé\n");
    exit(0);
}

/**
 * Exercise 8.7: Communication manager
 * 
 * Protocol specification:
 * - Main pipe T for manager communication
 * - Individual pipes Ti for each client
 * - Message types: SUBSCRIBE, SEND, LIST, UNSUBSCRIBE, ACK
 */
int main(void) {
    int manager_fd;
    struct message msg;
    
    // Set up signal handlers
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);
    
    printf("=== Gestionnaire de Communication ===\n");
    printf("Pipe principal: %s\n", MANAGER_FIFO);
    
    // Create manager pipe
    if (create_manager_pipe() == -1) {
        exit(EXIT_FAILURE);
    }
    
    // Open manager pipe for reading
    manager_fd = open(MANAGER_FIFO, O_RDONLY);
    if (manager_fd == -1) {
        perror("open manager fifo");
        unlink(MANAGER_FIFO);
        exit(EXIT_FAILURE);
    }
    
    printf("Gestionnaire prêt. En attente de messages...\n");
    
    // Main loop - wait for messages
    while (1) {
        ssize_t bytes_read = read(manager_fd, &msg, sizeof(struct message));
        
        if (bytes_read == -1) {
            perror("read manager fifo");
            break;
        }
        
        if (bytes_read != sizeof(struct message)) {
            printf("Message incomplet reçu\n");
            continue;
        }
        
        process_message(&msg);
    }
    
    close(manager_fd);
    unlink(MANAGER_FIFO);
    
    return 0;
}