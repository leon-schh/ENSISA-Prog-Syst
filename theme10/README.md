# Theme 10 - System: Memory Management

This folder contains all the exercises from Theme 10 (Système : Gestion mémoire) of the system programming exercises.

## Exercise Overview

### Ex10.1 - Type Sizes
**File:** `ex10_1.c`
**Description:** Displays the sizes of various C types on the current system (int, char, char*, struct, struct*). Explains struct padding for alignment requirements.

### Ex10.2 - Relative Addresses
**File:** `ex10_2.c`
**Description:** Demonstrates how to calculate the relative address of struct fields and implements the `ADRESSE_RELATIVE_DE` macro to obtain field offsets.

### Ex10.3 - Canary Memory Protection
**File:** `ex10_3.c`
**Description:** Implements a memory corruption detection system using "canary" values. Custom `mon_malloc` and `mon_free` functions add canary values before and after allocated memory to detect buffer overflows and other memory corruptions.

### Ex10.4 - Memory-Mapped File Reading
**File:** `ex10_4.c`
**Description:** Uses the `mmap` system call to display the first and last lines of a file. Demonstrates efficient file reading using memory mapping.

### Ex10.5 - Custom Memory Allocator
**File:** `ex10_5.c`
**Description:** Implements a complete memory allocator using the `sbrk` system call. Features:
- First Fit algorithm for allocation
- Free list management with automatic merging
- Memory alignment
- Block splitting and coalescing

## Compilation

To compile all exercises:
```bash
gcc -Wall -Wextra -std=c99 -g -o ex10_1 ex10_1.c
gcc -Wall -Wextra -std=c99 -g -o ex10_2 ex10_2.c
gcc -Wall -Wextra -std=c99 -g -o ex10_3 ex10_3.c
gcc -Wall -Wextra -std=c99 -g -o ex10_4 ex10_4.c
gcc -Wall -Wextra -std=c99 -g -o ex10_5 ex10_5.c
```

## Usage Examples

### Ex10.4 Usage:
```bash
./ex10_4 filename.txt
```

### Ex10.5 Memory Allocator Test:
```bash
./ex10_5
```

## Key Concepts Covered

1. **Memory Layout:** Understanding how data is organized in memory
2. **Struct Alignment:** Padding and packing considerations
3. **Memory Protection:** Techniques for detecting corruption
4. **System Calls:** Direct interaction with the OS for memory management
5. **Memory Management:** Building custom allocators with sophisticated algorithms

All exercises are fully functional and demonstrate advanced system programming concepts.
