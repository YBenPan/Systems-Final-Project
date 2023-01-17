#ifndef PARSER_H
#define PARSER_H

#include "table.h"
#include "vector.h"
#include "file_io.h"
#define MAX_CMD_LENGTH 255

#define checkInput(input) {if (!(input)) { printf("Error: argument not supplied!\n\n"); return 0; }}

int add_row_cmd(struct table * table, char *args);

void table_main(char *table_name);

int table_parser(char *table_name, char *input, int key);

int select_table(char *args);

int create_table(char *args);

int drop_table(char *args);

int global_parser(char *input);

#endif
