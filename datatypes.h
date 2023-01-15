#ifndef DATATYPES_H
#define DATATYPES_H

struct datatype{
	int type;
	int args;
};

#define DATATYPE_INT 0
#define DATATYPE_SMALL 1
#define DATATYPE_CHAR 2
#define DATATYPE_LONG 3
#define DATATYPE_FLOAT 4
#define DATATYPE_DOUBLE 5
#define DATATYPE_TEXT 6

// if size is 0, treats args as sizes
#define DATATYPE_SIZES {4, 2, 1, 8, 4, 8, 0}

#endif
