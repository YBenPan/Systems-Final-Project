# Language Specifications

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
