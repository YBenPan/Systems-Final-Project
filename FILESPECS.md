# File Specifications

Table files will be binary files:

NAME OF FILE: `[table_name].tbl`

## Version 0:

**HEADER:**

BYTES 0-3: TBLF [identifier so we know this is a table file]

BYTES 4-7: VERSION, int

BYTES 8-11: NUMBER OF COLUMNS, int - n

BYTES 12-15: NUMBER OF ROWS, int - m


**SECTION 1: COLUMN IDENTIFIERS**

START OFFSET: 16

SIZE: 64\*n

BYTES 16-(64\*n+15): 64 byte segments (string), i-th segment from 16+(64\*i+15) is the i-th column's name

**SECTION 2: TABLE DATA**

START OFFSET: 64\*n+16

SIZE: 4\*m\*n

ROW-MAJOR ORDER, TABLE[ROW][COL INDEX] -> STARTOFFSET + 4\*(n\*ROW + COL INDEX) to STARTOFFSET + 4\*(n\*ROW + COL INDEX) + 3 bytes
