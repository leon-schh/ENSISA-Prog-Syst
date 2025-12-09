/*
 * Exercice 5.10
 * Write a command that takes as parameter a directory name, and displays all
 * objects contained in this directory. We will take the same display restriction
 * conventions as the ls command (no display of names starting with a dot).
 */

#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <limits.h>

/* Compare function for qsort */
int compare_names(const void *a, const void *b) {
    const char *sa = *(const char **)a;
    const char *sb = *(const char **)b;
    return strcmp(sa, sb);
}

/* Check if filename should be hidden (starts with dot) */
int is_hidden_file(const char *name) {
    return name[0] == '.';
}

/* Get file type description */
const char* get_file_type(mode_t mode) {
    if (S_ISREG(mode)) return "";
    if (S_ISDIR(mode)) return "/";
    if (S_ISCHR(mode)) return "";
    if (S_ISBLK(mode)) return "";
    if (S_ISFIFO(mode)) return "|";
    if (S_ISLNK(mode)) return "@";
    if (S_ISSOCK(mode)) return "=";
    return "";
}

/* Display file information in ls-like format */
void display_file_info(const char *dirpath, const char *filename) {
    char fullpath[4096];
    struct stat st;
    
    snprintf(fullpath, sizeof(fullpath), "%s/%s", dirpath, filename);
    
    if (lstat(fullpath, &st) == -1) {
        fprintf(stderr, "Error getting info for %s: %s\n", fullpath, strerror(errno));
        return;
    }
    
    printf("%s%s\n", filename, get_file_type(st.st_mode));
}

/* List directory contents */
int list_directory(const char *dirpath) {
    DIR *dir;
    struct dirent *entry;
    char **names = NULL;
    int count = 0;
    int capacity = 16;
    
    /* Open directory */
    dir = opendir(dirpath);
    if (!dir) {
        fprintf(stderr, "Error opening directory '%s': %s\n", dirpath, strerror(errno));
        return -1;
    }
    
    /* Allocate initial array for names */
    names = malloc(capacity * sizeof(char *));
    if (!names) {
        fprintf(stderr, "Memory allocation error\n");
        closedir(dir);
        return -1;
    }
    
    /* Read directory entries */
    while ((entry = readdir(dir)) != NULL) {
        /* Skip hidden files (starting with dot) */
        if (is_hidden_file(entry->d_name)) {
            continue;
        }
        
        /* Expand array if needed */
        if (count >= capacity) {
            capacity *= 2;
            char **new_names = realloc(names, capacity * sizeof(char *));
            if (!new_names) {
                fprintf(stderr, "Memory allocation error\n");
                /* Clean up */
                for (int i = 0; i < count; i++) {
                    free(names[i]);
                }
                free(names);
                closedir(dir);
                return -1;
            }
            names = new_names;
        }
        
        /* Store filename */
        names[count] = strdup(entry->d_name);
        if (!names[count]) {
            fprintf(stderr, "Memory allocation error\n");
            /* Clean up */
            for (int i = 0; i < count; i++) {
                free(names[i]);
            }
            free(names);
            closedir(dir);
            return -1;
        }
        count++;
    }
    
    closedir(dir);
    
    /* Sort names */
    qsort(names, count, sizeof(char *), compare_names);
    
    /* Display files */
    printf("Directory contents of '%s':\n", dirpath);
    printf("=============================\n");
    
    for (int i = 0; i < count; i++) {
        display_file_info(dirpath, names[i]);
        free(names[i]);
    }
    
    printf("\nTotal: %d files\n", count);
    
    free(names);
    return 0;
}

/* Check if path is a directory */
int is_directory(const char *path) {
    struct stat st;
    if (stat(path, &st) == -1) {
        return 0;
    }
    return S_ISDIR(st.st_mode);
}

/* Display usage information */
void display_usage(const char *progname) {
    printf("Usage: %s <directory_path>\n", progname);
    printf("\nList all files in a directory (excluding hidden files).\n");
    printf("\nExample:\n");
    printf("  %s /tmp\n", progname);
    printf("  %s .\n", progname);
    printf("  %s /usr/bin\n", progname);
}

/* Main program */
int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Error: Directory path required\n");
        display_usage(argv[0]);
        return 1;
    }
    
    const char *dirpath = argv[1];
    
    /* Check if path exists */
    if (access(dirpath, F_OK) == -1) {
        fprintf(stderr, "Error: Path '%s' does not exist: %s\n", dirpath, strerror(errno));
        return 1;
    }
    
    /* Check if it's a directory */
    if (!is_directory(dirpath)) {
        fprintf(stderr, "Error: '%s' is not a directory\n", dirpath);
        return 1;
    }
    
    /* List directory contents */
    int result = list_directory(dirpath);
    
    if (result == -1) {
        fprintf(stderr, "Failed to list directory contents\n");
        return 1;
    }
    
    return 0;
}

/*
 * IMPLEMENTATION NOTES:
 * 
 * 1. Directory reading:
 *    - Uses opendir(), readdir(), closedir() system calls
 *    - Handles directory entry structure (struct dirent)
 *    - Proper error handling for all operations
 * 
 * 2. Hidden file filtering:
 *    - Skips files starting with '.' (like ls)
 *    - Can be easily modified to include hidden files
 * 
 * 3. Sorting:
 *    - Uses qsort() for alphabetical sorting
 *    - Allocates dynamic array to handle arbitrary number of files
 *    - Proper memory management with malloc/free
 * 
 * 4. File type indication:
 *    - Shows '/' for directories
 *    - Shows '@' for symbolic links
 *    - Shows '|' for FIFOs
 *    - Shows '=' for sockets
 *    - Shows nothing for regular files
 * 
 * 5. Error handling:
 *    - Checks if path exists
 *    - Verifies it's actually a directory
 *    - Handles permission errors
 *    - Reports all errors with descriptive messages
 * 
 * This implementation mimics the basic functionality of 'ls' command
 * while demonstrating direct use of directory system primitives.
 */