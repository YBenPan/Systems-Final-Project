#ifndef TABLE_H
#define TABLE_H

#include "vector.h"
#include "schema.h"

#define MAXIMUM_CHAR_COUNT_TABLE_NAME 64
#define MAXIMUM_COL_LENGTH 64
#define MAXIMUM_COL_COUNT 64

struct table{
  char name[MAXIMUM_CHAR_COUNT_TABLE_NAME];
  char (*columnnames)[MAXIMUM_COL_LENGTH];
  struct schema * schm;
  int colcount;
  int rowcount;
  struct vector * data; // vector of tablerow pointers
};

struct tablerow{
  struct schema * schm;
  char * data;
};

// automatically truncates names to 64 chars (including null terminator)
struct table * init_table(char * tablename, char **columnnames, int colcount, struct schema * schm);

void add_row(struct table * table, struct tablerow * row);

// text is 2d array of the same size as schm->colcount, i-th entry corresponds to i-th entry of schema
// returns 0 if success, 1 if failed
char add_row_from_text(struct table * table, struct schema * schm, char ** text);

void print_table(struct table * table);

void print_table_row(struct tablerow * tablerow);

#endif
