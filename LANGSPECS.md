# Language Specifications

Similar to SQL for now. Will update later to a more user-friendly version. 

Note: the commands below are reserved keywords. Tables / files / rows / columns are banned from using them. 

### SELECT

```
SELECT column_name 
FROM table_name 
```

### UPDATE

```
UPDATE table_name
SET some_column = some_value
WHERE some_column = some_value;
```

### DELETE

```
DELETE FROM table_name
WHERE some_column = some_value;
```

### INSERT INTO

```
INSERT INTO table_name (column_1, column_2, column_3) 
VALUES (value_1, 'value_2', value_3);
```

### CREATE TABLE

```
CREATE TABLE table_name (
  column_1 datatype, 
  column_2 datatype, 
  column_3 datatype
);
```

### ALTER TABLE

```
ALTER TABLE table_name 
ADD column_name datatype;
```
### DROP TABLE

```
DROP TABLE table_name;
```
