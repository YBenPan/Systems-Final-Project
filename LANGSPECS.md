# Language Specifications

Similar to SQL for now. Will update later to a more user-friendly version. 

Note: the commands below are reserved keywords. Tables / files / rows / columns are banned from using them. 

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
