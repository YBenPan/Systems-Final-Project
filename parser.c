#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "parser.h"

void chop_newline(char *s) {
  size_t ln = strlen(s) - 1;
  if (*s && s[ln] == '\n') 
    s[ln] = '\0';
}

int select_table(char *args) {
  // Process args and put together file path
  char *table_name = strsep(&args, " ");
  char *file_dir = "./";
  char *file = malloc(sizeof(table_name) + 1);
  strcpy(file, file_dir);
  file[strlen(file)] = '\0';
  file = strcat(file, table_name);
  // printf("%s\n", file);

  // Open table
  struct table * table = read_table(table_name);
  
  // Prompt for user input
  char *input_str[MAX_CMD_LENGTH];
  printf("Opened table '%s'. Input table command:\n", table_name);
  fgets(input_str, MAX_CMD_LENGTH, stdin);
  chop_newline(input_str);

  // Router
  if (!strcmp(input_str, "PRINT")) {
    print_table(table);
  }
  else if (!strcmp(input_str, "ADDROW")) { // TODO: Implement ADDROW and parse into vectors

  }
  else if (!strcmp(input_str, "DELROW")) { // TODO: Implement DELROW

  }
  else if (!strcmp(input_str, "SETROW")) { // TODO: Implement SETROW

  }
  else if (!strcmp(input_str, "UPDATE")) { // TODO: Implement UPDATE

  }
  else if (!strcmp(input_str, "ADDCOL")) { // TODO: Implement ADDCOL

  }
  else if (!strcmp(input_str, "DELCOL")) { // TODO: Implement DELCOL

  }
  else if (!strcmp(input_str, "QUERY")) { // TODO: Implement QUERY

  }
  else if (!strcmp(input_str, "SORT")) { // TODO: Implement SORT. Warning: advanced feature! 

  }
  else {
    // TODO: Ask user to try again instead, within SELECT
    printf("Invalid command '%s'!\n", input_str);
    exit(EXIT_FAILURE);
  }
}

int create_table(char *args) {
  // Process args and put together file path
  char *table_name = strsep(&args, " ");
  char *file_dir = "./";
  char *file = malloc(sizeof(table_name) + 1);
  strcpy(file, file_dir);
  file = strcat(file, table_name);
  // printf("%s\n", file);

  // 2D array for column names. See tabledebug.c
  char *col_names[MAXIMUM_CHAR_COUNT_TABLE_NAME];
  // table->column_names -> char (*col_names)[64] -> pointer (dynamic array) to 64-sized fixed-length char arrays
  char *col_input = malloc(MAX_CMD_LENGTH);
  printf("Input column names, separated by space:\n");
  fgets(col_input, MAX_CMD_LENGTH, stdin);
  chop_newline(col_input);

  // Parse the column names
  int col_cnt = 0;
  while (1) {
    char *col_name = strsep(&col_input, " ");
    if (!col_name) {
      break;
    }
    col_names[col_cnt] = malloc((strlen(col_name) + 1) * sizeof(*col_names[col_cnt]));
    strcpy(col_names[col_cnt++], col_name);
    // printf("%s\n", col_name);
  }

  // Initialize table and free col_names
  struct table * table = init_table(table_name, col_names, col_cnt);
  for(int i = 0; i < col_cnt; ++i){
    free(col_names[i]);
  }

  // Write table
  if (!write_table(table)) {
    free(table);
    printf("Table '%s' created successfully!\n", table_name);
    return 0;
  }
  else {
    printf("Creation of table '%s' failed: %s\n", table_name, strerror(errno));
    exit(EXIT_FAILURE);
  }
}

int drop_table(char *args) {
  // Process args and put together file path
  char *table_name = strsep(&args, " ");
  char *file_dir = "./";
  char *file = malloc(sizeof(table_name) + 1);
  strcpy(file, file_dir);
  strcat(file, table_name);
  char *file_ext = ".tbl";
  strcat(file, file_ext);
  // printf("%s\n", file);

  if (remove(file) == -1) {
    printf("Dropping table '%s' failed: %s\n", table_name, strerror(errno));
    exit(EXIT_FAILURE);
  }
  else {
    printf("Table '%s' dropped successfully!\n", table_name);
    return 0;
  }
}

void usr_input(char *input) {
  printf("Input global command to start:\n");
  fgets(input, MAX_CMD_LENGTH, stdin);
  chop_newline(input);
}

void master_parser(char *input) {
  // Create a copy of input string
  char *input_str = malloc(sizeof(input));
  strcpy(input_str, input);

  // Get the command
  char *cmd = strsep(&input_str, " ");
  // printf("%s\n", cmd);
  // printf("%s\n", input_str);

  if (!strcmp(cmd, "SELECT")) {
    select_table(input_str);
  }
  else if (!strcmp(cmd, "CREATE")) {
    create_table(input_str);
  }
  else if (!strcmp(cmd, "DROP")) {
    drop_table(input_str);
  }
  else {
    printf("Invalid command '%s'!\n", cmd);
    // TODO: Ask user to try again instead
    exit(EXIT_FAILURE);
  }
}
