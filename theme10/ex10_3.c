#define _XOPEN_SOURCE 500
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define CANARY_SIZE 8
#define CANARY_VALUE 0x0123456789ABCDEFUL

/* Structure to hold allocation info */
typedef struct allocation_info {
    size_t size;
    uint64_t canary_before;
    uint64_t canary_after;
    void *real_ptr;
} allocation_info;

/* Simple linked list to track allocations */
typedef struct allocation_node {
    allocation_info info;
    struct allocation_node *next;
} allocation_node;

static allocation_node *allocations = NULL;

/* Custom malloc with canary protection */
void* mon_malloc(size_t size) {
    if (size == 0) {
        return NULL;
    }
    
    /* Allocate space for: canary_before + user_data + canary_after + allocation_info */
    size_t total_size = CANARY_SIZE + size + CANARY_SIZE + sizeof(allocation_info);
    void *raw_ptr = malloc(total_size);
    
    if (raw_ptr == NULL) {
        return NULL;
    }
    
    /* Set up the canary values */
    uint64_t *canary_before = (uint64_t*)raw_ptr;
    *canary_before = CANARY_VALUE;
    
    /* User data starts after the first canary */
    void *user_ptr = (char*)raw_ptr + CANARY_SIZE;
    
    /* Second canary goes after user data */
    uint64_t *canary_after = (uint64_t*)((char*)user_ptr + size);
    *canary_after = CANARY_VALUE;
    
    /* Store allocation info at the end */
    allocation_info *info = (allocation_info*)((char*)canary_after + CANARY_SIZE);
    info->size = size;
    info->canary_before = CANARY_VALUE;
    info->canary_after = CANARY_VALUE;
    info->real_ptr = raw_ptr;
    
    /* Add to tracking list */
    allocation_node *node = (allocation_node*)malloc(sizeof(allocation_node));
    if (node == NULL) {
        free(raw_ptr);
        return NULL;
    }
    
    node->info = *info;
    node->next = allocations;
    allocations = node;
    
    return user_ptr;
}

/* Custom free with canary validation */
void mon_free(void *ptr) {
    if (ptr == NULL) {
        return;
    }
    
    /* Find the allocation in our tracking list */
    allocation_node **current = &allocations;
    while (*current != NULL) {
        if ((*current)->info.real_ptr == (char*)ptr - CANARY_SIZE) {
            allocation_node *to_free = *current;
            *current = (*current)->next;
            
            /* Validate canaries before freeing */
            uint64_t *canary_before = (uint64_t*)to_free->info.real_ptr;
            uint64_t *canary_after = (uint64_t*)((char*)ptr + to_free->info.size);
            
            if (*canary_before != CANARY_VALUE) {
                printf("ERROR: Canarie avant corrompue! (attendu: 0x%016lx, trouvé: 0x%016lx)\n", 
                       CANARY_VALUE, *canary_before);
            }
            
            if (*canary_after != CANARY_VALUE) {
                printf("ERROR: Canarie après corrompue! (attendu: 0x%016lx, trouvé: 0x%016lx)\n", 
                       CANARY_VALUE, *canary_after);
            }
            
            if (*canary_before == CANARY_VALUE && *canary_after == CANARY_VALUE) {
                printf("Allocation validée, libération...\n");
            }
            
            /* Free the actual memory */
            free(to_free->info.real_ptr);
            free(to_free);
            return;
        }
        current = &(*current)->next;
    }
    
    printf("ERROR: Tentative de libération d'un pointeur non alloué par mon_malloc\n");
}

/* Test function to demonstrate the canary system */
int main(void) {
    printf("Test du système de canaris pour la détection de corruption mémoire\n\n");
    
    /* Test 1: Normal allocation and freeing */
    printf("Test 1: Allocation et libération normales\n");
    char *ptr1 = (char*)mon_malloc(16);
    if (ptr1) {
        strcpy(ptr1, "Hello");
        printf("Allocated and filled: '%s'\n", ptr1);
        mon_free(ptr1);
    }
    
    /* Test 2: Allocation, corruption, and detection */
    printf("\nTest 2: Corruption détectée\n");
    char *ptr2 = (char*)mon_malloc(16);
    if (ptr2) {
        strcpy(ptr2, "Test");
        printf("Allocated: '%s'\n", ptr2);
        
        /* Simulate buffer overflow */
        printf("Simulating buffer overflow...\n");
        for (int i = 0; i < 25; i++) {
            ptr2[i] = 'X';  /* This will corrupt the canary */
        }
        
        mon_free(ptr2);  /* This should detect the corruption */
    }
    
    /* Test 3: Double free detection */
    printf("\nTest 3: Double free\n");
    char *ptr3 = (char*)mon_malloc(10);
    if (ptr3) {
        strcpy(ptr3, "Double");
        mon_free(ptr3);
        mon_free(ptr3);  /* This should fail */
    }
    
    return 0;
}
