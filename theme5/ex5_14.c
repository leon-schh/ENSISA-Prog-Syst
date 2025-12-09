/*
 * Exercice 5.14
 * Resume the function developed during exercise 3.16 to write a program which
 * to search where a command is found. For example, "which ls" should give: /bin/ls
 * 
 * Note: Exercise 3.16 was about parsing the PATH environment variable.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

#define MAX_PATHS 1024
#define MAX_PATH_LENGTH 4096
#define MAX_COMMAND_LENGTH 1024

/* Parse PATH environment variable and return array of directories */
int parse_path(char *paths[], int max_paths) {
    char *path_env;
    int count = 0;
    
    path_env = getenv("PATH");
    if (!path_env) {
        return 0;
    }
    
    /* Make a copy since strtok modifies the string */
    char *path_copy = strdup(path_env);
    if (!path_copy) {
        return 0;
    }
    
    /* Tokenize by ':' */
    char *token = strtok(path_copy, ":");
    while (token != NULL && count < max_paths) {
        paths[count] = strdup(token);
        if (!paths[count]) {
            /* Clean up on error */
            for (int i = 0; i < count; i++) {
                free(paths[i]);
            }
            free(path_copy);
            return 0;
        }
        count++;
        token = strtok(NULL, ":");
    }
    
    free(path_copy);
    return count;
}

/* Check if a file exists and is executable */
int is_executable(const char *filepath) {
    struct stat st;
    
    if (stat(filepath, &st) == -1) {
        return 0; /* File doesn't exist or can't be accessed */
    }
    
    /* Check if it's a regular file and has execute permission */
    return S_ISREG(st.st_mode) && (st.st_mode & S_IXUSR);
}

/* Find command in PATH */
char* find_command(const char *command) {
    static char *paths[MAX_PATHS];
    char fullpath[MAX_PATH_LENGTH];
    int path_count;
    
    /* Check if command contains path separator */
    if (strchr(command, '/') != NULL) {
        /* Command contains path, check directly */
        if (is_executable(command)) {
            return strdup(command);
        }
        return NULL;
    }
    
    /* Parse PATH environment variable */
    path_count = parse_path(paths, MAX_PATHS);
    if (path_count == 0) {
        return NULL;
    }
    
    /* Search in each PATH directory */
    for (int i = 0; i < path_count; i++) {
        snprintf(fullpath, sizeof(fullpath), "%s/%s", paths[i], command);
        
        if (is_executable(fullpath)) {
            /* Clean up path array and return full path */
            for (int j = 0; j < path_count; j++) {
                free(paths[j]);
            }
            return strdup(fullpath);
        }
    }
    
    /* Clean up path array */
    for (int i = 0; i < path_count; i++) {
        free(paths[i]);
    }
    
    return NULL;
}

/* Display PATH environment variable */
void display_path(void) {
    char *path_env = getenv("PATH");
    
    if (!path_env) {
        printf("PATH environment variable not set\n");
        return;
    }
    
    printf("PATH directories:\n");
    
    char *path_copy = strdup(path_env);
    if (!path_copy) {
        fprintf(stderr, "Memory allocation error\n");
        return;
    }
    
    char *token = strtok(path_copy, ":");
    int count = 1;
    while (token != NULL) {
        printf("  %d. %s\n", count++, token);
        token = strtok(NULL, ":");
    }
    
    free(path_copy);
}

/* Check if file exists (any type) */
int file_exists(const char *filepath) {
    struct stat st;
    return stat(filepath, &st) == 0;
}

/* Find all occurrences of a command */
void find_all_occurrences(const char *command) {
    static char *paths[MAX_PATHS];
    char fullpath[MAX_PATH_LENGTH];
    int path_count;
    int found_any = 0;
    
    printf("Searching for all occurrences of '%s':\n", command);
    
    /* Parse PATH environment variable */
    path_count = parse_path(paths, MAX_PATHS);
    if (path_count == 0) {
        printf("PATH environment variable not set or empty\n");
        return;
    }
    
    /* Search in each PATH directory */
    for (int i = 0; i < path_count; i++) {
        snprintf(fullpath, sizeof(fullpath), "%s/%s", paths[i], command);
        
        if (file_exists(fullpath)) {
            printf("  %s", fullpath);
            
            if (is_executable(fullpath)) {
                printf(" (executable)");
            } else {
                printf(" (not executable)");
            }
            printf("\n");
            found_any = 1;
        }
    }
    
    /* Clean up path array */
    for (int i = 0; i < path_count; i++) {
        free(paths[i]);
    }
    
    if (!found_any) {
        printf("  Command '%s' not found in PATH\n", command);
    }
}

/* Display usage information */
void display_usage(const char *progname) {
    printf("Usage: %s [options] <command>\n", progname);
    printf("\nFind the location of a command in the PATH.\n");
    printf("\nOptions:\n");
    printf("  -a    Show all occurrences of the command\n");
    printf("  -p    Show PATH directories\n");
    printf("\nExamples:\n");
    printf("  %s ls\n", progname);
    printf("  %s -a python\n", progname);
    printf("  %s -p\n", progname);
    printf("\nIf no options, shows the first executable found in PATH.\n");
}

/* Main program */
int main(int argc, char *argv[]) {
    int show_all = 0;
    int show_path = 0;
    const char *command = NULL;
    
    /* Parse command line arguments */
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-a") == 0) {
            show_all = 1;
        } else if (strcmp(argv[i], "-p") == 0) {
            show_path = 1;
        } else if (argv[i][0] == '-') {
            fprintf(stderr, "Unknown option: %s\n", argv[i]);
            display_usage(argv[0]);
            return 1;
        } else {
            command = argv[i];
        }
    }
    
    /* Handle special cases */
    if (show_path && !command) {
        display_path();
        return 0;
    }
    
    if (!command) {
        fprintf(stderr, "Error: Command name required\n");
        display_usage(argv[0]);
        return 1;
    }
    
    /* Find command */
    if (show_all) {
        find_all_occurrences(command);
    } else {
        char *result = find_command(command);
        if (result) {
            printf("%s\n", result);
            free(result);
            return 0;
        } else {
            fprintf(stderr, "%s: command not found\n", command);
            return 1;
        }
    }
    
    return 0;
}

/*
 * IMPLEMENTATION NOTES:
 * 
 * 1. PATH parsing:
 *    - Uses getenv() to get PATH environment variable
 *    - Tokenizes by ':' separator
 *    - Handles empty or missing PATH gracefully
 * 
 * 2. Command lookup:
 *    - Checks if command contains '/' (explicit path)
 *    - If yes, checks directly for existence and executability
 *    - If no, searches in each PATH directory in order
 * 
 * 3. File type checking:
 *    - Uses stat() to get file information
 *    - Checks for regular file (S_ISREG)
 *    - Checks execute permission (S_IXUSR)
 * 
 * 4. Options support:
 *    - -a: show all occurrences in PATH
 *    - -p: display PATH directories
 *    - Default: show first executable found
 * 
 * 5. Error handling:
 *    - Memory allocation errors
 *    - Missing PATH environment variable
 *    - Command not found
 *    - Invalid command line options
 * 
 * 6. Memory management:
 *    - Properly frees allocated memory
 *    - Uses strdup() for safe string copying
 *    - Static buffers for temporary storage
 * 
 * This implementation demonstrates:
 * - Environment variable handling
 * - String parsing and tokenization
 * - File system operations (stat, access)
 * - Command line argument processing
 * - Memory management and error handling
 * 
 * It's equivalent to the Unix 'which' command functionality.
 */