# Theme 6: System Process Management

This directory contains all the exercises from Theme 6 of the System Programming course, covering process management in Unix/Linux systems.

## Exercises Overview

### Exercise 6.1: Basic Fork Process
**File**: `ex6_1.c`
**Description**: Creates a child process using `fork()`. The child displays its PID and parent's PID, then exits with a return code equal to the last digit of its PID. The parent waits for the child and displays the return code.
**Usage**: `./ex6_1`

### Exercise 6.2: Multiple Child Processes
**File**: `ex6_2.c`
**Description**: Takes an integer `n` as argument and creates `n` child processes. Each child displays its number and PID, then exits with its index as return code. The parent waits for all children to terminate.
**Usage**: `./ex6_2 <n>`

### Exercise 6.3: Recursive Process Generation
**File**: `ex6_3.c`
**Description**: Implements `matprocn m` functionality. Creates a tree of processes with `n` processes at each level, recursively for `m` levels. Total processes generated: n^m.
**Usage**: `./ex6_3 <n>`

### Exercise 6.4: File Reading with Parent and Child
**File**: `ex6_4.c`
**Description**: Opens a file, creates a child process, and both processes (parent and child) read characters from the file using `read()`. Demonstrates file descriptor sharing after `fork()`.
**Usage**: `./ex6_4 <fichier>`

### Exercise 6.5: Directory Timing with ls -l
**File**: `ex6_5.c`
**Description**: Measures the execution time of the `ls -l` command on a given directory using `gettimeofday()`.
**Usage**: `./ex6_5 <repertoire>`

### Exercise 6.6: CPU Time Measurement with ls -R
**File**: `ex6_6.c`
**Description**: Measures the CPU time consumed by the `ls -R` command using the `times()` system primitive. Redirects output to `/dev/null` to focus on CPU usage.
**Usage**: `./ex6_6 <repertoire>`

### Exercise 6.7: Parallel Command Execution (parexec)
**File**: `ex6_7.c`
**Description**: Implements `parexec n cmd args...` functionality. Launches `n` instances of a command in parallel, adding the process index (0 to n-1) as the last argument to each instance.
**Usage**: `./ex6_7 <n> <cmd> [args...]`
**Example**: `./ex6_7 5 sh tester toto` executes:
- `sh tester toto 0`
- `sh tester toto 1`
- `sh tester toto 2`
- `sh tester toto 3`
- `sh tester toto 4`

### Exercise 6.8: Random Sleep Processes
**File**: `ex6_8.c`
**Description**: Generates `n` random numbers between 1 and `m`, creates `n` child processes, each sleeping for its assigned random duration. The parent waits and displays the return codes.
**Usage**: `./ex6_8 <n> <m>`

### Exercise 6.9: User Connection Check
**File**: `ex6_9.c`
**Description**: C program equivalent to the shell script:
```bash
ps eaux > toto; grep "^$1 " < toto > /dev/null && echo "$1 est connecté"
```
Uses `execlp()`, file redirections with `dup/dup2`, and `write()` for final output.
**Usage**: `./ex6_9 <utilisateur>`

### Exercise 6.10: Basic Shell Implementation
**File**: `ex6_10.c`
**Description**: Implements a simple shell that:
- Reads commands from stdin
- Parses commands into arguments
- Searches for commands in PATH
- Executes commands using `execv()`
- Supports 'exit' command to quit
**Usage**: `./ex6_10`

### Exercise 6.11: Shell with File Redirections
**File**: `ex6_11.c`
**Description**: Extends the basic shell to support file redirections:
- `< file` : redirect input from file
- `> file` : redirect output to file (overwrite)
- `>> file` : redirect output to file (append)
**Usage**: `./ex6_11`
**Example**: `ls > fichier.txt`, `cat < input.txt`

### Exercise 6.12: Shell with Background Processes
**File**: `ex6_12.c`
**Description**: Extends the shell to support background processes with `&` operator. Background processes don't block the shell prompt.
**Usage**: `./ex6_12`
**Example**: `long_command &`, `ls -R / > output.txt &`

## Compilation

All programs can be compiled with:
```bash
gcc -Wall -o <output_name> <source_file.c>
```

Or compile all at once:
```bash
for file in ex6_*.c; do
    gcc -Wall -o ${file%.c} $file
done
```

## Notes

- All programs include proper error handling
- Programs use standard POSIX system calls
- Memory management is handled properly (no leaks)
- Code follows Unix/Linux programming conventions
- Each exercise demonstrates specific process management concepts

## Testing

To test the programs:

1. **Basic fork test**: `./ex6_1`
2. **Multiple processes**: `./ex6_2 3`
3. **File operations**: Create a test file, then `./ex6_4 testfile.txt`
4. **Shell testing**: `./ex6_12` then try various commands with redirections
5. **Background processes**: `./ex6_12` then `sleep 5 &`

These exercises cover fundamental Unix process management concepts including process creation, synchronization, communication, and basic shell functionality.