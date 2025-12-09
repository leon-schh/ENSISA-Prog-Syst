/*
 * Exercice 5.15
 * Some shells (ksh, zsh, bash, etc.) have a CDPATH variable. This specifies
 * a number of search directories. When using cd with an argument (relative path name),
 * it is searched in the different directories indicated by CDPATH and the directory
 * change is performed if it is found.
 * 
 * Program a chdir command.
 * 
 * Why cannot this command work?
 */

/*
 * EXPLANATION OF WHY chdir() CANNOT WORK AS AN EXTERNAL COMMAND:
 * 
 * The chdir() system call changes the current working directory of the
 * CALLING PROCESS. When we run an external program (like our chdir command),
 * it runs in a NEW PROCESS that is a child of the shell.
 * 
 * Key points:
 * 1. Each process has its own current working directory
 * 2. When the chdir command exits, we return to the parent shell process
 * 3. The shell's working directory was never changed
 * 4. Only the child process (our chdir command) changed its directory
 * 
 * This is why chdir must be a BUILTIN command in the shell, not an external program.
 * 
 * However, we can demonstrate the functionality for educational purposes.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <errno.h>

#define MAX_PATH_LENGTH 4096

/* Parse CDPATH environment variable */
int parse_cdpath(char *paths[], int max_paths) {
    char *cdpath_env;
    int count = 0;
    
    cdpath_env = getenv("CDPATH");
    if (!cdpath_env) {
        /* If CDPATH is not set, use current directory as default */
        paths[0] = strdup(".");
        return 1;
    }
    
    /* Make a copy since strtok modifies the string */
    char *cdpath_copy = strdup(cdpath_env);
    if (!cdpath_copy) {
        return 0;
    }
    
    /* Tokenize by ':' */
    char *token = strtok(cdpath_copy, ":");
    while (token != NULL && count < max_paths) {
        paths[count] = strdup(token);
        if (!paths[count]) {
            /* Clean up on error */
            for (int i = 0; i < count; i++) {
                free(paths[i]);
            }
            free(cdpath_copy);
            return 0;
        }
        count++;
        token = strtok(NULL, ":");
    }
    
    free(cdpath_copy);
    return count;
}

/* Check if directory exists */
int directory_exists(const char *path) {
    struct stat st;
    if (stat(path, &st) == -1) {
        return 0;
    }
    return S_ISDIR(st.st_mode);
}

/* Get current working directory */
char* get_current_directory(void) {
    static char cwd[MAX_PATH_LENGTH];
    if (getcwd(cwd, sizeof(cwd)) == NULL) {
        return NULL;
    }
    return cwd;
}

/* Display CDPATH */
void display_cdpath(void) {
    char *cdpath = getenv("CDPATH");
    
    if (!cdpath) {
        printf("CDPATH is not set (using current directory '.' as default)\n");
    } else {
        printf("CDPATH: %s\n", cdpath);
    }
}

/* Change directory with CDPATH support */
int change_directory(const char *target) {
    static char *paths[1024];
    char fullpath[MAX_PATH_LENGTH];
    char resolved_path[MAX_PATH_LENGTH];
    int path_count;
    
    /* Handle special cases */
    if (target == NULL || strlen(target) == 0) {
        /* cd with no arguments goes to home directory */
        target = getenv("HOME");
        if (!target) {
            fprintf(stderr, "cd: HOME environment variable not set\n");
            return -1;
        }
    }
    
    /* Handle absolute paths */
    if (target[0] == '/') {
        if (chdir(target) == -1) {
            fprintf(stderr, "cd: %s: %s\n", target, strerror(errno));
            return -1;
        }
        printf("Changed to: %s\n", target);
        return 0;
    }
    
    /* Try direct relative path first */
    if (chdir(target) == 0) {
        printf("Changed to: %s (direct)\n", target);
        return 0;
    }
    
    /* Parse CDPATH and try each directory */
    path_count = parse_cdpath(paths, 1024);
    if (path_count == 0) {
        fprintf(stderr, "cd: failed to parse CDPATH\n");
        return -1;
    }
    
    /* Try each CDPATH directory */
    for (int i = 0; i < path_count; i++) {
        snprintf(fullpath, sizeof(fullpath), "%s/%s", paths[i], target);
        
        /* Resolve the path to get the canonical name */
        if (realpath(fullpath, resolved_path) != NULL) {
            if (chdir(resolved_path) == 0) {
                printf("Changed to: %s (via CDPATH: %s)\n", resolved_path, paths[i]);
                
                /* Clean up */
                for (int j = 0; j < path_count; j++) {
                    free(paths[j]);
                }
                return 0;
            }
        }
    }
    
    /* Clean up */
    for (int i = 0; i < path_count; i++) {
        free(paths[i]);
    }
    
    fprintf(stderr, "cd: %s: No such file or directory\n", target);
    return -1;
}

/* Display usage information */
void display_usage(const char *progname) {
    printf("Usage: %s [directory]\n", progname);
    printf("\nChange directory with CDPATH support.\n");
    printf("\nArguments:\n");
    printf("  directory    Directory to change to (optional)\n");
    printf("              If omitted, goes to HOME directory\n");
    printf("\nCDPATH Support:\n");
    printf("  If directory is a relative path and doesn't exist in current directory,\n");
    printf("  searches in directories listed in CDPATH environment variable.\n");
    printf("\nExamples:\n");
    printf("  %s              # Go to HOME\n", progname);
    printf("  %s /tmp         # Go to /tmp\n", progname);
    printf("  %s src          # Go to ./src or CDPATH/src\n", progname);
    printf("\nNote: This demonstrates chdir() functionality but cannot change\n");
    printf("      the shell's working directory since it runs in a separate process.\n");
    printf("      Use the shell's built-in 'cd' command for actual directory changes.\n");
}

/* Main program */
int main(int argc, char *argv[]) {
    char *target_dir = NULL;
    char *current_dir;
    
    printf("Educational chdir Command (CDPATH Support)\n");
    printf("==========================================\n\n");
    
    /* Parse arguments */
    if (argc > 2) {
        fprintf(stderr, "Error: Too many arguments\n");
        display_usage(argv[0]);
        return 1;
    } else if (argc == 2) {
        target_dir = argv[1];
    }
    
    /* Display current state */
    current_dir = get_current_directory();
    if (current_dir) {
        printf("Current directory: %s\n", current_dir);
    }
    
    display_cdpath();
    printf("\n");
    
    /* Perform directory change */
    if (change_directory(target_dir) == 0) {
        /* Show the new directory */
        current_dir = get_current_directory();
        if (current_dir) {
            printf("New current directory: %s\n", current_dir);
        }
    }
    
    printf("\n");
    printf("IMPORTANT NOTE:\n");
    printf("===============\n");
    printf("This command demonstrates chdir() functionality but cannot change\n");
    printf("the shell's working directory because it runs in a separate process.\n");
    printf("When this program exits, control returns to the shell, which still\n");
    printf("has its original working directory.\n");
    printf("\n");
    printf("To actually change the shell's directory, use the built-in 'cd' command:\n");
    printf("  cd /path/to/directory\n");
    printf("  cd src    # Uses CDPATH if available\n");
    
    return 0;
}

/*
 * DETAILED EXPLANATION OF WHY chdir() CANNOT WORK AS EXTERNAL COMMAND:
 * 
 * 1. PROCESS ISOLATION:
 *    - Each process has its own current working directory
 *    - External commands run in child processes
 *    - Child process changes don't affect parent process
 * 
 * 2. SYSTEM CALL BEHAVIOR:
 *    - chdir() system call affects ONLY the calling process
 *    - When the child process exits, the parent shell is unaffected
 *    - The shell continues with its original working directory
 * 
 * 3. FORK EXECUTION MODEL:
 *    - Shell forks() to create child process
 *    - Child process calls exec() to run the external command
 *    - Child process calls chdir() (this works but only for child)
 *    - Child process exits
 *    - Parent shell continues with unchanged working directory
 * 
 * 4. SHELL BUILTIN SOLUTION:
 *    - Shell built-in commands run in the shell's process itself
 *    - When shell executes 'cd', it directly calls chdir() in its own process
 *    - The shell's working directory is permanently changed
 *    - This is why 'cd' must be a shell builtin, not an external command
 * 
 * 5. CDPATH IMPLEMENTATION:
 *    - CDPATH is a shell environment variable (like PATH)
 *    - Contains colon-separated list of directories
 *    - Shell searches these directories for relative paths
 *    - Only works when implemented as shell builtin
 * 
 * DEMONSTRATION PURPOSE:
 * - Shows how chdir() and CDPATH functionality work
 * - Educational value for understanding process model
 * - Demonstrates environment variable handling
 * - Illustrates the difference between system calls and external programs
 * 
 * PRACTICAL IMPLICATIONS:
 * - External commands cannot permanently change the shell's state
 * - Only shell builtins can modify shell environment
 * - This applies to: cd, export, alias, etc.
 * - Understanding this is crucial for shell scripting and system programming
 */