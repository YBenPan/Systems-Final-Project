# Directions

`make serverprog` and `./serverprog` on server shell

`make clientprog` (or optionally `make clientprog ARGS=[host_ip]`) and `./clientprog` (or optionally `./clientprog [host_ip]`) on client shell
 
Follow the prompts. CTRL-C to exit.

# Working Features

- Sockets for server interactions with multiple clients
  - Server forks a subserver for each new client
  - Server kills all subservers when being killed
  - Client can send an internal `TERMINATE` command to kill subservers* (known bug: subservers remain as zombie processes)
  - Client knows when to expect user input when server sends a special integer "nonce" (`0xFF1111`), this doesn't interfere with regular output from server as characters are non-printable and therefore shouldn't occur in normal operation
- Semaphores to control read/write access to database tables
  - Each table creates a unique semaphore. Table names and correspoding semaphore keys are stored in a binary file `sem`
  - Initial value of each semaphore = SEM_INT_MAX
  - All clients can read a table at the same time: -1 to the semaphore
  - Writing blocks all access to the table (read and write): -(SEM_INT_MAX - 1) to the semaphore
- Implementation of database tables using 2D vectors
  - Implementation of 1D and 2D vectors using struct
  - Implementation of two vector structs, `struct vector` which stores `void *` pointers, and `struct intvector` which stores integers (in the final version, the latter is only used for storing child PIDs to kill fo the server)
  - Each table stores a **schema** that encodes the number of columns and data type of each column using an elaborate offset system described with a `int *`, data stored in `struct tablerow`'s in a `char *` buffer
  - Various commands to modify tables (see section below)
  - Data types of a column include INT, SMALLINT, TINYINT, LONG, FLOAT, DOUBLE, CHAR, and TEXT(n), where n is the maximum length of the text. Implemented using a `char *` buffer as arbitrary memory, and storing relevant values in the schema
  - File I/O into binary files
  - File output into CSV files* (code exists, however there is no command for it in the final version, however the `tabledebug` program does provide a demo-able version of it)
- Parsers
  - Parser to handle user input and direct it to functions at two tiers: global and table-specific commands 
  - Parser to convert user input (string) to datatype object
  - Parser to convert user input (string) to schema object

# Working Commands (see LANGSPECS.md for detail)

## Global Commands (Tier 1)

### SELECT: open table.

```
SELECT table_name
```

### CREATE: create table.

```
CREATE table_name
```

### DROP: drop table.

```
DROP table_name
```

## Table Commands (Tier 2)

### PRINT: Output table or schema.
```
PRINT SCHEMA
PRINT TABLE
```

### ADDROW: append a row to the table. Must match schema.
Note: there is an intentional 3-second delay to test if semaphores work. 
```
ADDROW (value_1 value_2 value_3)
```

### DELROW: delete a row with specified index.

```
DELROW row_index
```

### SETROW: update a row with specified index. Must match schema.
```
SETROW row_index (value_1 value_2 value_3)
```

### ADDCOL: add a column. Program will then prompt for data type and default value of the column. 
```
ADDCOL col_name
```

### EXIT: exit table-specific shell.
```
EXIT
```

# Known Bugs
- CREATE table_name overwrites current table if it already exists. *Not really* a bug
- Killing the server doesn't kill the clients by itself, however clients do end after receiving a broken pipe
- Killing the client doesn't fully kill the corresponding subserver, leaves it as a zombie process
- Likely many memory leaks, notably tables aren't freed after you switch tables / save them
- Trying to access one of the sample tables fails since they do not have a semaphore entry configured (they were added before we added semaphores)
- Trying to access a table that doesn't exist fails with the wrong error message (one that is semaphore-related0
