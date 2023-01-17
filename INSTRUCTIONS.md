# Directions

`make serverprog` and `./serverprog` on server shell

`make clientprog` and `./clientprog` on client shell
 
Follow the prompts. 

# Working Features

- Sockets for server, subserver, and multiple client interactions 
- Semaphores to control read/write access to database tables
  - Each table creates a unique semaphore. Table names and correspoding semaphore keys are stored in a binary file `sem`
  - Initial value of each semaphore = SEM_INT_MAX
  - All clients can read a table at the same time: -1 to the semaphore
  - Writing blocks all access to the table (read and write): -(SEM_INT_MAX - 1) to the semaphore
- Implementation of tables using 2D vectors
  - Implementation of 1D and 2D vectors using struct
  - Each table stores a schema that encodes the number of columns and data type of each column in a string using an offset system
  - Various commands to modify tables (see section below)
  - Data types of a column include INT, LONG, CHAR, TEXT(n), where n is the number of bytes

# Working Commands (see LANGSPECS.md for detail)

## Global Commands (Tier 1)

### SELECT

```
SELECT table_name
```

### CREATE

```
CREATE table_name
```

### DROP

```
DROP table_name
```

## Table Commands (Tier 2)

### PRINT
```
PRINT SCHEMA
PRINT TABLE
```

### ADDROW

```
ADDROW (value_1 value_2 value_3)
```

### DELROW:

```
DELROW row_index
```

### SETROW: 
```
SETROW row_index (value_1 value_2 value_3)
```

### ADDCOL:
```
ADDCOL col_name
```




# Known Bugs
