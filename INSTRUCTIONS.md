# Directions

`make serverprog` and `./serverprog` on server shell

`make clientprog` and `./clientprog` on client shell
 
Follow the prompts. CTRL-C to exit.

# Working Features

- Sockets for server interactions with multiple clients
  - Server forks a subserver for each new client
- Semaphores to control read/write access to database tables
  - Each table creates a unique semaphore. Table names and correspoding semaphore keys are stored in a binary file `sem`
  - Initial value of each semaphore = SEM_INT_MAX
  - All clients can read a table at the same time: -1 to the semaphore
  - Writing blocks all access to the table (read and write): -(SEM_INT_MAX - 1) to the semaphore
- Implementation of database tables using 2D vectors
  - Implementation of 1D and 2D vectors using struct
  - Each table stores a **schema** that encodes the number of columns and data type of each column using an elaborate offset system stored in `char *`
  - Various commands to modify tables (see section below)
  - Data types of a column include INT, SMALLINT, TINYINT, LONG, FLOAT, DOUBLE, CHAR, and TEXT(n), where n is the maximum length of the text. Implemented using 
  - File I/O into binary files
- Parsers
  - Parser to handle user input and direct it to functions at two tiers: global and table-specific commands 
  - Parser to convert user input to datatype object
  - Parser to convert user input to schema object

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
