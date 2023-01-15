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

void recompute_row_byte_size(struct schema * schm);

#endif
