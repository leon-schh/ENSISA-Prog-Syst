/*
 * Exercice 5.12
 * Resume the previous program to restore in the copies the access and modification
 * dates as well as the permissions of the original files.
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
#include <utime.h>
#include <time.h>

#define BUFFER_SIZE 8192

/* Copy a regular file with metadata preservation */
int copy_file_with_metadata(const char *src_path, const char *dst_path) {
    int fd_src, fd_dst;
    char buffer[BUFFER_SIZE];
    ssize_t bytes_read, bytes_written;
    struct stat st;
    struct utimbuf times;
    
    /* Get source file metadata */
    if (lstat(src_path, &st) == -1) {
        perror(src_path);
        return -1;
    }
    
    /* Open source file */
    fd_src = open(src_path, O_RDONLY);
    if (fd_src == -1) {
        perror(src_path);
        return -1;
    }
    
    /* Create destination file with source permissions */
    fd_dst = open(dst_path, O_WRONLY | O_CREAT | O_TRUNC, st.st_mode);
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
    
    /* Preserve access and modification times */
    times.actime = st.st_atime;   /* access time */
    times.modtime = st.st_mtime;  /* modification time */
    
    if (utime(dst_path, &times) == -1) {
        fprintf(stderr, "Warning: Failed to set times for %s: %s\n", 
                dst_path, strerror(errno));
    }
    
    /* Preserve additional metadata */
    if (chown(dst_path, st.st_uid, st.st_gid) == -1) {
        fprintf(stderr, "Warning: Failed to set owner/group for %s: %s\n", 
                dst_path, strerror(errno));
    }
    
    return 0;
}

/* Create a directory with preserved permissions */
int create_directory_with_metadata(const char *path, mode_t mode, uid_t uid, gid_t gid) {
    if (mkdir(path, mode) == -1) {
        perror(path);
        return -1;
    }
    
    /* Set ownership */
    if (chown(path, uid, gid) == -1) {
        fprintf(stderr, "Warning: Failed to set owner/group for %s: %s\n", 
                path, strerror(errno));
    }
    
    return 0;
}

/* Set directory times */
void set_directory_times(const char *path, struct stat *st) {
    struct utimbuf times;
    
    times.actime = st->st_atime;
    times.modtime = st->st_mtime;
    
    if (utime(path, &times) == -1) {
        fprintf(stderr, "Warning: Failed to set times for %s: %s\n", 
                path, strerror(errno));
    }
}

/* Copy directory recursively with metadata preservation */
int copy_directory_with_metadata(const char *src_dir, const char *dst_dir) {
    DIR *dir;
    struct dirent *entry;
    struct stat st;
    char src_path[4096];
    char dst_path[4096];
    
    printf("Copying directory: %s -> %s\n", src_dir, dst_dir);
    
    /* Get source directory metadata */
    if (lstat(src_dir, &st) == -1) {
        perror(src_dir);
        return -1;
    }
    
    /* Create destination directory with source permissions */
    if (create_directory_with_metadata(dst_dir, st.st_mode, st.st_uid, st.st_gid) == -1) {
        return -1;
    }
    
    /* Set directory access and modification times */
    set_directory_times(dst_dir, &st);
    
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
            printf("  Copying file: %s (permissions: %04o)\n", entry->d_name, st.st_mode & 0777);
            if (copy_file_with_metadata(src_path, dst_path) == -1) {
                fprintf(stderr, "Failed to copy file: %s\n", entry->d_name);
            }
            
        } else if (S_ISDIR(st.st_mode)) {
            /* Subdirectory */
            printf("  Copying subdirectory: %s (permissions: %04o)\n", entry->d_name, st.st_mode & 0777);
            if (copy_directory_with_metadata(src_path, dst_path) == -1) {
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
                /* Note: symlinks don't have traditional permissions or times */
            } else {
                perror("readlink");
            }
            
        } else {
            /* Other file types (devices, pipes, etc.) - skip with warning */
            printf("  Skipping special file: %s (type not supported for metadata preservation)\n", entry->d_name);
        }
    }
    
    closedir(dir);
    return 0;
}

/* Display metadata comparison */
void compare_metadata(const char *src_path, const char *dst_path) {
    struct stat src_st, dst_st;
    
    if (lstat(src_path, &src_st) == -1 || lstat(dst_path, &dst_st) == -1) {
        return;
    }
    
    printf("Metadata comparison for %s:\n", basename((char*)src_path));
    printf("  Source permissions: %04o, Dest permissions: %04o\n", 
           src_st.st_mode & 0777, dst_st.st_mode & 0777);
    printf("  Source owner: %d:%d, Dest owner: %d:%d\n", 
           src_st.st_uid, src_st.st_gid, dst_st.st_uid, dst_st.st_gid);
    printf("  Source access time: %s", ctime(&src_st.st_atime));
    printf("  Dest access time:   %s", ctime(&dst_st.st_atime));
    printf("  Source mod time:    %s", ctime(&src_st.st_mtime));
    printf("  Dest mod time:      %s", ctime(&dst_st.st_mtime));
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
    printf("\nCopy an entire directory tree with metadata preservation.\n");
    printf("Preserves: permissions, ownership, access time, modification time\n");
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
    
    printf("Directory Tree Copy Tool with Metadata Preservation\n");
    printf("===================================================\n\n");
    
    /* Copy the directory tree with metadata */
    if (copy_directory_with_metadata(src_dir, dst_dir) == -1) {
        fprintf(stderr, "Failed to copy directory tree\n");
        return 1;
    }
    
    printf("\nDirectory tree copied successfully with metadata preservation!\n");
    
    /* Display metadata comparison for the root directory */
    printf("\nMetadata verification:\n");
    compare_metadata(src_dir, dst_dir);
    
    return 0;
}

/*
 * IMPLEMENTATION NOTES:
 * 
 * 1. Metadata preservation:
 *    - Permissions: copied during file/directory creation
 *    - Ownership (UID/GID): set with chown()
 *    - Access time: set with utime()
 *    - Modification time: set with utime()
 * 
 * 2. System calls used:
 *    - utime(): set access and modification times
 *    - chown(): set file ownership
 *    - lstat(): get file metadata without following symlinks
 *    - mkdir(): create directories
 * 
 * 3. File type considerations:
 *    - Regular files: full metadata preservation
 *    - Directories: permissions, ownership, and times
 *    - Symbolic links: copied as links (times don't apply)
 *    - Special files: may not support all metadata operations
 * 
 * 4. Error handling:
 *    - Warns if metadata cannot be preserved (but continues)
 *    - Files are still copied even if metadata fails
 *    - Uses lstat to avoid following symlinks accidentally
 * 
 * 5. Verification:
 *    - Compares metadata between source and destination
 *    - Shows permissions, ownership, and timestamps
 *    - Helps verify successful metadata preservation
 * 
 * This implementation demonstrates advanced filesystem operations
 * including metadata manipulation and preservation techniques.
 */