#ifndef FILE_IO_H
#define FILE_IO_H

#include "table.h"

// REMEMBER TO UPDATE THIS WHEN YOU CHANGE FILE FORMAT
#define CURRENT_TABLE_FILE_VERSION 0

// returns 0 if success
char write_table(struct table * table);

struct table * read_table(char * table_name);

#endif
