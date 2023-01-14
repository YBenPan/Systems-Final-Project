#ifndef PARSER_H
#define PARSER_H

#include "table.h"
#include "vector.h"
#include "file_io.h"
#define MAX_CMD_LENGTH 255

void chop_newline(char *s);

int add_row_cmd(char *args);

void table_parser(struct table * table);

int select_table(char *args);

int create_table(char *args);

int drop_table(char *args);

void usr_input(char *input);

void global_parser(char *input);

#endif
