# Theme 8 - Système : Tubes (System: Pipes)

This directory contains all the implementations of Theme 8 exercises from the system programming course, focusing on pipe communication between processes.

## Exercises Overview

### Exercise 8.1 - Basic Pipe Communication
**File**: `ex8_1.c`

Demonstrates basic pipe communication between a father and son process:
- Father reads from standard input and writes to pipe
- Son reads from pipe and writes to standard output
- Father waits for son to complete

**Usage**:
```bash
gcc ex8_1.c -o ex8_1
./ex8_1 < inputfile > outputfile
# or
./ex8_1 < /bin/ls > toto
cmp /bin/ls toto  # Should show no difference
```

### Exercise 8.2 - Chain of N Processes
**File**: `ex8_2.c`

Generalizes the pipe concept to a chain of n processes:
- Creates n-1 pipes for n processes
- Each process passes data to the next in the chain
- All children are direct children of the main process

**Usage**:
```bash
gcc ex8_2.c -o ex8_2
./ex8_2 <number_of_processes>
./ex8_2 5  # Creates 5 processes in a chain
```

### Exercise 8.3 - Pipe Capacity Measurement
**File**: `ex8_3.c`

Determines the capacity of a pipe by:
- Creating a pipe with a reader that never reads
- Writing data until the writer blocks
- Using signals to display the capacity when writer blocks

**Usage**:
```bash
gcc ex8_3.c -o ex8_3
./ex8_3
# In another terminal, send signal to see capacity:
kill -USR1 <PID>
```

### Exercise 8.4 - Shell Command Pipeline
**File**: `ex8_4.c`

Implements the equivalent of the shell command:
```bash
ps eaux | grep "<username>" | wc -l
```

**Usage**:
```bash
gcc ex8_4.c -o ex8_4
./ex8_4 <username>  # Search for specific user
./ex8_4             # Uses USER environment variable
```

### Exercise 8.5 - Advanced Shell with Pipes
**File**: `ex8_5.c`

Creates a shell that supports:
- Pipe chains (`cmd1 | cmd2 | cmd3`)
- Background execution (`cmd &`)
- Command parsing and execution

**Usage**:
```bash
gcc ex8_5.c -o ex8_5
./ex8_5
> ls -l | grep .c | wc -l &
> quit
```

### Exercise 8.6 - Named Pipes (FIFO)
**Files**: `ex8_6_writer.c`, `ex8_6_reader.c`

Reimplements exercise 8.1 using named pipes (FIFOs):
- Two separate programs instead of fork
- Named pipe created with `mkfifo`
- Can be run independently

**Usage**:
```bash
# Terminal 1 (Writer)
gcc ex8_6_writer.c -o writer
./writer < inputfile

# Terminal 2 (Reader) - run after writer starts
gcc ex8_6_reader.c -o reader
./reader > outputfile
```

### Exercise 8.7 - Communication Manager System
**File**: `ex8_7_manager.c`

Implements a centralized communication manager:
- Manages inter-process communication through named pipes
- Handles client subscription/unsubscription
- Routes messages between clients
- Supports broadcast and point-to-point messaging

**Message Protocol**:
- Main pipe `gestionnaire_T` for manager communication
- Individual pipes `T_i` for each client
- Message types: SUBSCRIBE, SEND, LIST, UNSUBSCRIBE, ACK

**Usage**:
```bash
gcc ex8_7_manager.c -o manager
./manager
```

### Exercise 8.8 - Client Processes
**File**: `ex8_8_client.c`

Implements client processes that work with the communication manager:
- Defines the `comm` communication type
- Implements communication functions: `initialiser`, `liste`, `envoyer`, `recevoir`
- Simple test application: finds minimum PID among all connected clients

**Usage**:
```bash
# Terminal 1 - Start manager
./manager

# Terminal 2, 3, ... - Start multiple clients
gcc ex8_8_client.c -o client
./client gestionnaire_T

# Each client will:
# - Subscribe to manager
# - Get list of all clients
# - Find minimum PID
# - Exchange test messages
```

## Key Concepts Demonstrated

1. **Anonymous Pipes**: Inter-process communication within related processes
2. **Named Pipes (FIFO)**: Communication between unrelated processes
3. **Process Chains**: Data passing through multiple processes
4. **Pipe Capacity**: Understanding OS pipe buffer limitations
5. **Shell Pipeline Implementation**: Recreating shell pipe functionality
6. **Communication Manager**: Centralized message routing system
7. **Client-Server Architecture**: Structured inter-process communication

## Compilation

All programs can be compiled with:
```bash
gcc -Wall -Wextra -o <output_name> <source_file.c>
```

## Dependencies

- POSIX-compliant system (Linux/Unix)
- Standard C library
- System calls: pipe(), fork(), exec(), mkfifo(), open(), read(), write()

## Notes

- Exercises 8.7 and 8.8 work together as a complete communication system
- Named pipes are created in the current directory
- Some exercises require multiple terminals to demonstrate full functionality
- Error handling is included but could be enhanced for production use