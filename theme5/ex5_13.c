/*
 * Exercice 5.13
 * Write a program to display all names of files in a directory tree whose
 * content contains a character string. Your program must have the following syntax:
 * 
 * chercherchaine répertoire
 * 
 * For example "chercher "struct utimbuf" /usr/include" should display:
 * /usr/include/utime.h
 * /usr/include/linux/utime.h
 * ....
 * 
 * You will use system primitives to search for the string in each file, using
 * an efficient method (i.e. not reading byte by byte). To display the name
 * of each file found, you can use the printf library function.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#include <signal.h>

#define BUFFER_SIZE 65536  /* 64KB buffer for efficient reading */
#define MAX_SEARCH_STRING 1024

/* Global flag for interruption */
volatile sig_atomic_t interrupted = 0;

/* Signal handler for Ctrl+C */
void interrupt_handler(int sig) {
    interrupted = 1;
}

/* Check if file should be skipped */
int should_skip_file(const char *filename) {
    /* Skip common binary files and system files */
    const char *skip_patterns[] = {
        ".so", ".a", ".o", ".bin", ".img", ".iso",
        "core", ".tmp", ".temp", ".swp", ".swo"
    };
    
    for (size_t i = 0; i < sizeof(skip_patterns) / sizeof(skip_patterns[0]); i++) {
        if (strstr(filename, skip_patterns[i]) != NULL) {
            return 1;
        }
    }
    return 0;
}

/* Check if file is likely binary */
int is_binary_file(const char *filepath) {
    int fd = open(filepath, O_RDONLY);
    if (fd == -1) return 0; /* Assume text if can't open */
    
    unsigned char buffer[1024];
    ssize_t bytes_read = read(fd, buffer, sizeof(buffer));
    close(fd);
    
    if (bytes_read <= 0) return 0;
    
    /* Check for null bytes or high proportion of non-printable chars */
    int null_bytes = 0;
    int non_printable = 0;
    
    for (ssize_t i = 0; i < bytes_read; i++) {
        if (buffer[i] == 0) {
            null_bytes++;
        }
        if (buffer[i] < 7 || (buffer[i] > 13 && buffer[i] < 32)) {
            non_printable++;
        }
    }
    
    /* If more than 10% non-printable or any null bytes, likely binary */
    return (null_bytes > 0) || (non_printable > bytes_read / 10);
}

/* Search for string in file using efficient buffer-based method */
int search_string_in_file(const char *filepath, const char *search_string) {
    int fd;
    char *buffer = NULL;
    char *search_pos;
    size_t buffer_size = BUFFER_SIZE;
    size_t search_len = strlen(search_string);
    ssize_t bytes_read;
    size_t total_read = 0;
    int found = 0;
    
    if (search_len == 0) return 0;
    if (should_skip_file(filepath)) return 0;
    if (is_binary_file(filepath)) return 0;
    
    fd = open(filepath, O_RDONLY);
    if (fd == -1) return 0;
    
    buffer = malloc(buffer_size);
    if (!buffer) {
        close(fd);
        return 0;
    }
    
    /* Search using sliding window approach */
    size_t search_window = search_len * 2; /* Keep overlap for pattern matching */
    size_t prev_bytes = 0;
    
    while ((bytes_read = read(fd, buffer + prev_bytes, buffer_size - prev_bytes)) > 0) {
        if (interrupted) {
            free(buffer);
            close(fd);
            return -1;
        }
        
        bytes_read += prev_bytes;
        total_read += bytes_read;
        
        /* Search in current buffer */
        for (size_t i = 0; i <= bytes_read - search_len; i++) {
            if (memcmp(buffer + i, search_string, search_len) == 0) {
                found = 1;
                break;
            }
        }
        
        if (found) break;
        
        /* Keep last search_window bytes for next iteration */
        if (bytes_read >= search_window) {
            prev_bytes = search_window;
            memmove(buffer, buffer + bytes_read - prev_bytes, prev_bytes);
        } else {
            prev_bytes = bytes_read;
        }
    }
    
    free(buffer);
    close(fd);
    return found;
}

/* Search recursively in directory tree */
int search_in_directory(const char *dirpath, const char *search_string) {
    DIR *dir;
    struct dirent *entry;
    struct stat st;
    char fullpath[4096];
    int files_checked = 0;
    int files_found = 0;
    
    dir = opendir(dirpath);
    if (!dir) {
        fprintf(stderr, "Error opening directory %s: %s\n", dirpath, strerror(errno));
        return -1;
    }
    
    while ((entry = readdir(dir)) != NULL) {
        if (interrupted) break;
        
        /* Skip . and .. */
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }
        
        snprintf(fullpath, sizeof(fullpath), "%s/%s", dirpath, entry->d_name);
        
        /* Get file info */
        if (lstat(fullpath, &st) == -1) {
            continue;
        }
        
        if (S_ISDIR(st.st_mode)) {
            /* Recurse into subdirectory */
            int result = search_in_directory(fullpath, search_string);
            if (result == -1) {
                /* Error in subdirectory, but continue */
            }
        } else if (S_ISREG(st.st_mode)) {
            /* Search in regular file */
            files_checked++;
            
            if (files_checked % 100 == 0) {
                printf("Checked %d files...\r", files_checked);
                fflush(stdout);
            }
            
            int found = search_string_in_file(fullpath, search_string);
            if (found == 1) {
                printf("%s\n", fullpath);
                files_found++;
            }
        }
        /* Skip other file types (symlinks, devices, etc.) */
    }
    
    closedir(dir);
    printf("Checked %d files, found %d matches\n", files_checked, files_found);
    return files_found;
}

/* Display usage information */
void display_usage(const char *progname) {
    printf("Usage: %s <search_string> <directory>\n", progname);
    printf("\nSearch for files containing a specific string in a directory tree.\n");
    printf("\nArguments:\n");
    printf("  search_string  Text to search for (use quotes if it contains spaces)\n");
    printf("  directory      Root directory to search in\n");
    printf("\nExample:\n");
    printf("  %s \"main()\" .\n", progname);
    printf("  %s \"struct utimbuf\" /usr/include\n", progname);
    printf("  %s \"TODO\" ~/projects\n", progname);
    printf("\nNote: Press Ctrl+C to interrupt the search.\n");
}

/* Check if path is a directory */
int is_directory(const char *path) {
    struct stat st;
    if (stat(path, &st) == -1) {
        return 0;
    }
    return S_ISDIR(st.st_mode);
}

/* Main program */
int main(int argc, char *argv[]) {
    const char *search_string;
    const char *search_dir;
    
    /* Set up signal handler for graceful interruption */
    signal(SIGINT, interrupt_handler);
    
    if (argc != 3) {
        fprintf(stderr, "Error: Search string and directory required\n");
        display_usage(argv[0]);
        return 1;
    }
    
    search_string = argv[1];
    search_dir = argv[2];
    
    /* Validate search string length */
    if (strlen(search_string) >= MAX_SEARCH_STRING) {
        fprintf(stderr, "Error: Search string too long (max %d characters)\n", MAX_SEARCH_STRING - 1);
        return 1;
    }
    
    /* Check if directory exists */
    if (access(search_dir, F_OK) == -1) {
        fprintf(stderr, "Error: Directory '%s' does not exist: %s\n", 
                search_dir, strerror(errno));
        return 1;
    }
    
    /* Check if it's actually a directory */
    if (!is_directory(search_dir)) {
        fprintf(stderr, "Error: '%s' is not a directory\n", search_dir);
        return 1;
    }
    
    printf("File Search Tool\n");
    printf("================\n");
    printf("Searching for: \"%s\"\n", search_string);
    printf("In directory: %s\n", search_dir);
    printf("Press Ctrl+C to interrupt...\n\n");
    
    /* Start search */
    int result = search_in_directory(search_dir, search_string);
    
    if (interrupted) {
        printf("\n\nSearch interrupted by user.\n");
        return 1;
    }
    
    if (result < 0) {
        fprintf(stderr, "Search completed with errors.\n");
        return 1;
    }
    
    if (result == 0) {
        printf("No files found containing the search string.\n");
    } else {
        printf("\nSearch completed. Found %d file(s) containing the search string.\n", result);
    }
    
    return 0;
}

/*
 * IMPLEMENTATION NOTES:
 * 
 * 1. Efficient searching:
 *    - Uses large buffers (64KB) to minimize system calls
 *    - Sliding window approach to handle patterns across buffer boundaries
 *    - Only searches regular files (excludes directories, devices, etc.)
 * 
 * 2. Binary file detection:
 *    - Checks for null bytes and high proportion of non-printable characters
 *    - Skips likely binary files to improve performance
 *    - Uses heuristic based on first 1KB of file
 * 
 * 3. Progress reporting:
 *    - Shows progress every 100 files checked
 *    - Displays total files checked and matches found
 *    - Allows interruption with Ctrl+C
 * 
 * 4. Error handling:
 *    - Gracefully handles permission errors
 *    - Continues searching even if individual files can't be read
 *    - Reports errors but doesn't stop the search
 * 
 * 5. Performance considerations:
 *    - Skips common binary file patterns (.so, .a, .o, etc.)
 *    - Uses buffered I/O instead of character-by-character reading
 *    - Only processes files that are likely to contain text
 * 
 * This implementation demonstrates efficient file searching techniques
 * and proper handling of large directory trees with progress reporting.
 */