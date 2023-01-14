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
  printf("%s\n", file);

  // Open table
  int fd = open(file, O_RDONLY);
  if (fd == -1) {
    printf("Table '%s' not found!\n", table_name);
    exit(errno);
  }

  // TODO: Select table. If statements to direct function to more operations

  close(fd);
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
  char *col_input = malloc(sizeof(MAX_CMD_LENGTH));
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
    col_names[col_cnt] = malloc(strlen(col_name) + 1);
    strcpy(col_names[col_cnt++], col_name);
    // printf("%s\n", col_name);
  }
  struct table * table = init_table(table_name, col_names, col_cnt);
  if (!write_table(table)) {
    printf("Table '%s' created successfully!\n", table_name);
    return 0;
  }
  else {
    printf("Table creation failed: %s\n", strerror(errno));
    exit(EXIT_FAILURE);
  }
}

int drop_table(char *args) {

}

void usr_input(char *input) {
  printf("Input table command:\n");
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
    exit(EXIT_FAILURE);
  }
}
