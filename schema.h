#ifndef SCHEMA_H
#define SCHEMA_H

#include "vector.h"

struct schema{
	int colcount;
	/*
    array of size (colcount + 1) containing partial sums of byte sizes
    rowbytesize[i] is the memory offset (in bytes) of the START of the i-th data value
    rowbytesize[colcount] is the TOTAL NUMBER OF BYTES used, ie. where you would add a new element
	*/
	int * rowbytesize;
	struct vector * datatypes;
};

struct schema * init_schema(int colcount, struct vector * datatypes);

// colcount -> expected column count, text -> space separated string of datatypes, if error, returns NULL
struct schema * init_schema_from_text(int colcount, char * text);

void recompute_rowbytesize(struct schema * schm);

void print_schema(struct schema * schm);

#endif
