#ifndef TABLE_H
#define TABLE_H

#include "vector.h"

struct table{
	char name[64];
	char (*columnnames)[64];
	int colcount;
	int rowcount;
	struct vector * data; // vector of intvector pointers
};

// automatically truncates names to 64 chars (including null terminator)
struct table * init_table(char * tablename, char **columnnames, int colcount);

void add_row(struct table * table, struct intvector * row);

void print_table(struct table * table);

#endif
