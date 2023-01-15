#ifndef DATATYPES_H
#define DATATYPES_H

struct datatype{
	int type;
	int args;
};

#define TOTAL_DATATYPE_COUNT 8

#define DATATYPE_INT 0
#define DATATYPE_SMALLINT 1
#define DATATYPE_TINYINT 2
#define DATATYPE_LONG 3
#define DATATYPE_FLOAT 4
#define DATATYPE_DOUBLE 5
#define DATATYPE_CHAR 6
#define DATATYPE_TEXT 7

// if size is 0, treats args as sizes
#define DATATYPE_SIZES {4, 2, 1, 8, 4, 8, 1, 0}

#define DATATYPE_LABELS {"int", "smallint", "tinyint", "long", "float", "double", "char", "text"}

#define DATATYPE_REQ_ARGUMENTS {0, 0, 0, 0, 0, 0, 0, 1}

int get_datatype_size(struct datatype * dt);

char * parse_string_to_data(char * strinput, struct datatype * dt);

#endif
