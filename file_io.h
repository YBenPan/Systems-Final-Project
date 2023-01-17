#ifndef FILE_IO_H
#define FILE_IO_H

#include "table.h"

// REMEMBER TO UPDATE THIS WHEN YOU CHANGE FILE FORMAT
#define CURRENT_TABLE_FILE_VERSION 1

#define TABLE_FILE_BINARY_ROW_ALIGNMENT 8

#define CSV_ESCAPE_CHARACTER '"'
#define CSV_DELIMITER_CHARACTER ','

int get_alignment_padding(int raw_byte_count);

// returns 0 if success
char write_table(struct table * table);

struct table * read_table(char * table_name);

// returns 0 if success
char write_table_to_csv(struct table * table, char * output_file);

char * csv_ify(char *term);

#endif
