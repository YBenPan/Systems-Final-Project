#ifndef TABLE_H
#define TABLE_H

#include "vector.h"

#define MAXIMUM_CHAR_COUNT_TABLE_NAME 64
#define MAXIMUM_COL_COUNT 64

struct table{
  char name[MAXIMUM_CHAR_COUNT_TABLE_NAME];
  char (*columnnames)[MAXIMUM_COL_COUNT];
  int colcount;
  int rowcount;
  struct vector * data; // vector of intvector pointers
};

// automatically truncates names to 64 chars (including null terminator)
struct table * init_table(char * tablename, char **columnnames, int colcount);

void add_row(struct table * table, struct intvector * row);

void print_table(struct table * table);

#endif
