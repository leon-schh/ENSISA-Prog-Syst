/*
 * Exercice 5.11
 * Resume the program from exercise 5.4 to copy an entire directory tree.
 * Your program will take two directory names as parameters: an existing
 * directory name to copy and a destination directory name (which we can
 * assume does not exist for simplification).
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
#include <libgen.h>

#define BUFFER_SIZE 8192

/* Copy a regular file */
int copy_file(const char *src_path, const char *dst_path) {
    int fd_src, fd_dst;
    char buffer[BUFFER_SIZE];
    ssize_t bytes_read, bytes_written;
    
    /* Open source file */
    fd_src = open(src_path, O_RDONLY);
    if (fd_src == -1) {
        perror(src_path);
        return -1;
    }
    
    /* Create destination file */
    fd_dst = open(dst_path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd_dst == -1) {
        perror(dst_path);
        close(fd_src);
        return -1;
    }
    
    /* Copy file contents */
    while ((bytes_read = read(fd_src, buffer, BUFFER_SIZE)) > 0) {
        bytes_written = write(fd_dst, buffer, bytes_read);
        if (bytes_written != bytes_read) {
            perror("write");
            close(fd_src);
            close(fd_dst);
            return -1;
        }
    }
    
    if (bytes_read == -1) {
        perror("read");
        close(fd_src);
        close(fd_dst);
        return -1;
    }
    
    close(fd_src);
    close(fd_dst);
    return 0;
}

/* Create a directory with given permissions */
int create_directory(const char *path, mode_t mode) {
    if (mkdir(path, mode) == -1) {
        perror(path);
        return -1;
    }
    return 0;
}

/* Copy directory recursively */
int copy_directory(const char *src_dir, const char *dst_dir) {
    DIR *dir;
    struct dirent *entry;
    struct stat st;
    char src_path[4096];
    char dst_path[4096];
    
    printf("Copying directory: %s -> %s\n", src_dir, dst_dir);
    
    /* Get source directory info */
    if (lstat(src_dir, &st) == -1) {
        perror(src_dir);
        return -1;
    }
    
    /* Create destination directory */
    if (create_directory(dst_dir, st.st_mode) == -1) {
        return -1;
    }
    
    /* Open source directory */
    dir = opendir(src_dir);
    if (!dir) {
        perror(src_dir);
        return -1;
    }
    
    /* Process each entry */
    while ((entry = readdir(dir)) != NULL) {
        /* Skip . and .. */
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }
        
        /* Build full paths */
        snprintf(src_path, sizeof(src_path), "%s/%s", src_dir, entry->d_name);
        snprintf(dst_path, sizeof(dst_path), "%s/%s", dst_dir, entry->d_name);
        
        /* Get entry info */
        if (lstat(src_path, &st) == -1) {
            fprintf(stderr, "Error getting info for %s: %s\n", src_path, strerror(errno));
            continue;
        }
        
        if (S_ISREG(st.st_mode)) {
            /* Regular file */
            printf("  Copying file: %s\n", entry->d_name);
            if (copy_file(src_path, dst_path) == -1) {
                fprintf(stderr, "Failed to copy file: %s\n", entry->d_name);
            }
            
        } else if (S_ISDIR(st.st_mode)) {
            /* Subdirectory */
            printf("  Copying subdirectory: %s\n", entry->d_name);
            if (copy_directory(src_path, dst_path) == -1) {
                fprintf(stderr, "Failed to copy directory: %s\n", entry->d_name);
            }
            
        } else if (S_ISLNK(st.st_mode)) {
            /* Symbolic link */
            char link_target[4096];
            ssize_t link_len = readlink(src_path, link_target, sizeof(link_target) - 1);
            if (link_len != -1) {
                link_target[link_len] = '\0';
                printf("  Copying symbolic link: %s -> %s\n", entry->d_name, link_target);
                if (symlink(link_target, dst_path) == -1) {
                    perror("symlink");
                }
            } else {
                perror("readlink");
            }
            
        } else {
            /* Other file types (devices, pipes, etc.) - skip with warning */
            printf("  Skipping special file: %s (type not supported)\n", entry->d_name);
        }
    }
    
    closedir(dir);
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
    printf("Usage: %s <source_directory> <destination_directory>\n", progname);
    printf("\nCopy an entire directory tree.\n");
    printf("The destination directory must not exist.\n");
    printf("\nExample:\n");
    printf("  %s /path/to/source /path/to/destination\n", progname);
    printf("  %s ./mydir ./mydir_backup\n", progname);
}

/* Main program */
int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Error: Source and destination directories required\n");
        display_usage(argv[0]);
        return 1;
    }
    
    const char *src_dir = argv[1];
    const char *dst_dir = argv[2];
    
    /* Check if source directory exists */
    if (access(src_dir, F_OK) == -1) {
        fprintf(stderr, "Error: Source directory '%s' does not exist: %s\n", 
                src_dir, strerror(errno));
        return 1;
    }
    
    /* Check if source is actually a directory */
    if (!is_directory(src_dir)) {
        fprintf(stderr, "Error: '%s' is not a directory\n", src_dir);
        return 1;
    }
    
    /* Check if destination already exists */
    if (access(dst_dir, F_OK) == -1) {
        /* Destination doesn't exist - good */
    } else {
        fprintf(stderr, "Error: Destination directory '%s' already exists\n", dst_dir);
        return 1;
    }
    
    printf("Directory Tree Copy Tool\n");
    printf("========================\n\n");
    
    /* Copy the directory tree */
    if (copy_directory(src_dir, dst_dir) == -1) {
        fprintf(stderr, "Failed to copy directory tree\n");
        return 1;
    }
    
    printf("\nDirectory tree copied successfully!\n");
    return 0;
}

/*
 * IMPLEMENTATION NOTES:
 * 
 * 1. Recursive copying:
 *    - Uses recursion to handle nested directories
 *    - Preserves directory structure
 *    - Processes each entry individually
 * 
 * 2. File type handling:
 *    - Regular files: copied with file content
 *    - Directories: created and recursed into
 *    - Symbolic links: copied as links (not dereferenced)
 *    - Special files: skipped with warning
 * 
 * 3. Error handling:
 *    - Checks source existence and type
 *    - Verifies destination doesn't exist
 *    - Reports errors for each file individually
 *    - Continues copying even if individual files fail
 * 
 * 4. Path handling:
 *    - Uses fixed-size buffers for paths (sufficient for most cases)
 *    - Builds full paths by concatenating directory and filename
 *    - Avoids buffer overflows with snprintf
 * 
 * 5. Progress reporting:
 *    - Shows which directories and files are being processed
 *    - Indicates symbolic link targets
 *    - Warns about skipped special files
 * 
 * This implementation demonstrates:
 * - Directory traversal with opendir/readdir/closedir
 * - Recursive programming with filesystem operations
 * - Different file types and how to handle them
 * - Proper error handling in complex operations
 */