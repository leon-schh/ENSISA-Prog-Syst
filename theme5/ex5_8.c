/*
 * Exercice 5.8
 * Write a program that displays in clear text the type of the requested file
 * (directory, ordinary file, etc.), as well as its permissions (read, write and
 * execute) in octal (base 8, with the %o format of printf).
 * 
 * Optionally: display the permissions in the same form as the ls command with
 * the -l option.
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <pwd.h>
#include <grp.h>

/* Convert file type to string */
const char* get_file_type(mode_t mode) {
    if (S_ISREG(mode)) return "regular file";
    if (S_ISDIR(mode)) return "directory";
    if (S_ISCHR(mode)) return "character device";
    if (S_ISBLK(mode)) return "block device";
    if (S_ISFIFO(mode)) return "FIFO/pipe";
    if (S_ISLNK(mode)) return "symbolic link";
    if (S_ISSOCK(mode)) return "socket";
    return "unknown";
}

/* Get symbolic permissions string like ls -l */
void get_symbolic_permissions(mode_t mode, char *perm_str) {
    /* Owner permissions */
    perm_str[0] = (mode & S_IRUSR) ? 'r' : '-';
    perm_str[1] = (mode & S_IWUSR) ? 'w' : '-';
    perm_str[2] = (mode & S_IXUSR) ? 'x' : '-';
    
    /* Group permissions */
    perm_str[3] = (mode & S_IRGRP) ? 'r' : '-';
    perm_str[4] = (mode & S_IWGRP) ? 'w' : '-';
    perm_str[5] = (mode & S_IXGRP) ? 'x' : '-';
    
    /* Other permissions */
    perm_str[6] = (mode & S_IROTH) ? 'r' : '-';
    perm_str[7] = (mode & S_IWOTH) ? 'w' : '-';
    perm_str[8] = (mode & S_IXOTH) ? 'x' : '-';
    
    perm_str[9] = '\0';
}

/* Get file type character for ls-like display */
char get_type_char(mode_t mode) {
    if (S_ISREG(mode)) return '-';
    if (S_ISDIR(mode)) return 'd';
    if (S_ISCHR(mode)) return 'c';
    if (S_ISBLK(mode)) return 'b';
    if (S_ISFIFO(mode)) return 'p';
    if (S_ISLNK(mode)) return 'l';
    if (S_ISSOCK(mode)) return 's';
    return '?';
}

/* Display file information */
void display_file_info(const char *filename) {
    struct stat st;
    char permissions[10];
    char full_permissions[12];
    struct passwd *pwd;
    struct group *grp;
    
    if (lstat(filename, &st) == -1) {
        perror(filename);
        return;
    }
    
    /* Basic information */
    printf("File: %s\n", filename);
    printf("Type: %s\n", get_file_type(st.st_mode));
    printf("Permissions (octal): %05o\n", st.st_mode & 0777);
    
    /* Symbolic permissions */
    get_symbolic_permissions(st.st_mode, permissions);
    printf("Permissions (symbolic): %s\n", permissions);
    
    /* ls -l style permissions */
    full_permissions[0] = get_type_char(st.st_mode);
    strcpy(full_permissions + 1, permissions);
    printf("Permissions (ls -l style): %s\n", full_permissions);
    
    /* Additional information */
    printf("Inode: %lu\n", (unsigned long)st.st_ino);
    printf("Size: %ld bytes\n", (long)st.st_size);
    printf("Blocks: %ld\n", (long)st.st_blocks);
    printf("Block size: %ld bytes\n", (long)st.st_blksize);
    
    /* Owner and group names */
    pwd = getpwuid(st.st_uid);
    grp = getgrgid(st.st_gid);
    
    if (pwd) {
        printf("Owner: %s (UID: %d)\n", pwd->pw_name, st.st_uid);
    } else {
        printf("Owner: UID %d\n", st.st_uid);
    }
    
    if (grp) {
        printf("Group: %s (GID: %d)\n", grp->gr_name, st.st_gid);
    } else {
        printf("Group: GID %d\n", st.st_gid);
    }
    
    /* Timestamps */
    printf("Access time: %s", ctime(&st.st_atime));
    printf("Modify time: %s", ctime(&st.st_mtime));
    printf("Change time: %s", ctime(&st.st_ctime));
    
    /* File flags */
    printf("Special flags:\n");
    if (st.st_mode & S_ISUID) printf("  Set-UID bit\n");
    if (st.st_mode & S_ISGID) printf("  Set-GID bit\n");
    if (st.st_mode & 01000) printf("  Sticky bit\n");
    
    /* Link count */
    printf("Hard links: %ld\n", (long)st.st_nlink);
}

/* Display usage information */
void display_usage(const char *progname) {
    printf("Usage: %s <file1> [file2] ...\n", progname);
    printf("Display file type and permissions information.\n");
    printf("\nExample:\n");
    printf("  %s /bin/ls /tmp /dev/null\n", progname);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Error: No files specified\n");
        display_usage(argv[0]);
        return 1;
    }
    
    printf("File Information Tool\n");
    printf("=====================\n\n");
    
    for (int i = 1; i < argc; i++) {
        display_file_info(argv[i]);
        if (i < argc - 1) {
            printf("\n");
        }
    }
    
    return 0;
}

/*
 * PERMISSION BITS EXPLANATION:
 * 
 * Octal permissions:
 * - 4 = read (r)
 * - 2 = write (w)
 * - 1 = execute (x)
 * 
 * Three groups of three bits each:
 * - Owner (user) permissions: bits 6,3,0
 * - Group permissions: bits 5,2,-1
 * - Other permissions: bits 4,1,-2
 * 
 * Example: 0755 means:
 * - Owner: rwx (7 = 4+2+1)
 * - Group: r-x (5 = 4+0+1)
 * - Other: r-x (5 = 4+0+1)
 * 
 * Special bits:
 * - S_ISUID (04000): Set user ID on execution
 * - S_ISGID (02000): Set group ID on execution
 * - S_ISVTX (01000): Sticky bit
 */