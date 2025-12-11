#define _XOPEN_SOURCE 500
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define PAGE_SIZE 4096
#define ALIGNMENT 8

/* Block structure for memory management */
typedef struct block {
    size_t size;           /* Size of the block (excluding header) */
    int free;              /* 1 if block is free, 0 if allocated */
    struct block *next;    /* Next block in the free list */
} block_t;

static block_t *free_list = NULL;  /* Head of the free block list */

/* Align size to the nearest multiple of ALIGNMENT */
static size_t align_size(size_t size) {
    return (size + ALIGNMENT - 1) & ~(ALIGNMENT - 1);
}

/* Get the size of a page-aligned allocation request */
static size_t get_page_aligned_size(size_t size) {
    size_t total_size = sizeof(block_t) + size;
    size_t pages_needed = (total_size + PAGE_SIZE - 1) / PAGE_SIZE;
    return pages_needed * PAGE_SIZE;
}

/* Add a block to the free list (keeping it sorted by address) */
static void add_to_free_list(block_t *block) {
    block->free = 1;
    
    /* If list is empty, add as first element */
    if (free_list == NULL) {
        free_list = block;
        block->next = NULL;
        return;
    }
    
    /* Insert in sorted order by address */
    block_t **current = &free_list;
    while (*current != NULL && (size_t)*current < (size_t)block) {
        current = &(*current)->next;
    }
    
    block->next = *current;
    *current = block;
}

/* Merge adjacent free blocks */
static void merge_free_blocks() {
    block_t *current = free_list;
    
    while (current != NULL && current->next != NULL) {
        /* Check if current and next are adjacent */
        if ((char*)current + sizeof(block_t) + current->size == (char*)current->next) {
            /* Merge them */
            current->size += sizeof(block_t) + current->next->size;
            current->next = current->next->next;
        } else {
            current = current->next;
        }
    }
}

/* Find a free block using First Fit algorithm */
static block_t* find_free_block(size_t size) {
    block_t *current = free_list;
    
    while (current != NULL) {
        if (current->free && current->size >= size) {
            return current;
        }
        current = current->next;
    }
    
    return NULL;
}

/* Custom malloc implementation */
void* mon_malloc(size_t size) {
    if (size == 0) {
        return NULL;
    }
    
    /* Align the requested size */
    size_t aligned_size = align_size(size);
    
    /* Try to find a suitable free block */
    block_t *block = find_free_block(aligned_size);
    
    if (block != NULL) {
        /* Found a free block, allocate it */
        block->free = 0;
        
        /* If the block is much larger than needed, split it */
        size_t remaining_size = block->size - aligned_size;
        if (remaining_size > sizeof(block_t) + ALIGNMENT) {
            block_t *new_block = (block_t*)((char*)(block + 1) + aligned_size);
            new_block->size = remaining_size - sizeof(block_t);
            new_block->free = 1;
            new_block->next = block->next;
            
            block->size = aligned_size;
            block->next = new_block;
        }
        
        return (void*)(block + 1);
    }
    
    /* No suitable free block found, request more memory from system */
    size_t page_aligned_size = get_page_aligned_size(aligned_size);
    void *brk_ptr = sbrk(page_aligned_size);
    
    if (brk_ptr == (void*)-1) {
        return NULL;  /* sbrk failed */
    }
    
    /* Set up the new block */
    block = (block_t*)brk_ptr;
    block->size = page_aligned_size - sizeof(block_t);
    block->free = 0;
    block->next = NULL;
    
    /* If there's leftover space, create a free block */
    size_t leftover_size = block->size - aligned_size;
    if (leftover_size > sizeof(block_t) + ALIGNMENT) {
        block_t *free_block = (block_t*)((char*)(block + 1) + aligned_size);
        free_block->size = leftover_size - sizeof(block_t);
        free_block->free = 1;
        free_block->next = NULL;
        
        block->size = aligned_size;
        block->next = free_block;
        
        add_to_free_list(free_block);
    }
    
    return (void*)(block + 1);
}

/* Custom free implementation */
void mon_free(void *ptr) {
    if (ptr == NULL) {
        return;
    }
    
    /* Get the block header */
    block_t *block = (block_t*)ptr - 1;
    
    /* Mark as free */
    block->free = 1;
    
    /* Add to free list */
    add_to_free_list(block);
    
    /* Merge adjacent free blocks */
    merge_free_blocks();
}

/* Function to print memory allocation status (for debugging) */
void print_memory_status() {
    printf("Memory status:\n");
    printf("Free list:\n");
    
    block_t *current = free_list;
    int count = 0;
    size_t total_free = 0;
    
    while (current != NULL) {
        printf("  Block %d: address=%p, size=%zu, free=%d\n", 
               count, (void*)current, current->size, current->free);
        if (current->free) {
            total_free += current->size;
        }
        current = current->next;
        count++;
    }
    
    printf("Total free memory: %zu bytes\n", total_free);
}

/* Test program */
int main() {
    printf("Test du gestionnaire mémoire personnalisé\n\n");
    
    /* Test basic allocations */
    printf("Test 1: Allocations de base\n");
    char *ptr1 = (char*)mon_malloc(100);
    char *ptr2 = (char*)mon_malloc(200);
    char *ptr3 = (char*)mon_malloc(50);
    
    if (ptr1) strcpy(ptr1, "Hello");
    if (ptr2) strcpy(ptr2, "World");
    if (ptr3) strcpy(ptr3, "Test");
    
    printf("ptr1: %s\n", ptr1);
    printf("ptr2: %s\n", ptr2);
    printf("ptr3: %s\n", ptr3);
    
    print_memory_status();
    
    /* Test freeing and reusing */
    printf("\nTest 2: Libération et réutilisation\n");
    mon_free(ptr2);
    print_memory_status();
    
    /* Allocate a smaller block */
    char *ptr4 = (char*)mon_malloc(100);
    if (ptr4) {
        strcpy(ptr4, "Reused");
        printf("ptr4: %s\n", ptr4);
    }
    
    print_memory_status();
    
    /* Test alignment */
    printf("\nTest 3: Test d'alignement\n");
    void *ptr5 = mon_malloc(1);
    void *ptr6 = mon_malloc(1);
    printf("ptr5: %p (should be 8-byte aligned)\n", ptr5);
    printf("ptr6: %p (should be 8-byte aligned)\n", ptr6);
    
    /* Clean up */
    mon_free(ptr1);
    mon_free(ptr3);
    mon_free(ptr4);
    mon_free(ptr5);
    mon_free(ptr6);
    
    printf("\nFinal memory status:\n");
    print_memory_status();
    
    return 0;
}
