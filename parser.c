#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "parser.h"

void chop_newline(char *s) {
  size_t ln = strlen(s) - 1;
  if (*s && s[ln] == '\n') 
    s[ln] = '\0';
}

int add_row_cmd(struct table * table, char *args) {  
  // Check syntax "(...)" and remove parentheses
  if (args[0] != '(' || args[strlen(args) - 1] != ')') {
    printf("Syntax Error: Row must be surrounded by parentheses!\n");
    exit(EXIT_FAILURE);
  }
  strsep(&args, "(");
  args[strlen(args) - 1] = '\0';
  
  // Form vector
  struct intvector *row = init_intvector();
  char *row_item; int row_num; 

  // Deconstruct user input into row elements
  while ((row_item = strsep(&args, " "))) {
    sscanf(row_item, "%d", &row_num);
    add_intvector(row, row_num); 
  }

  // Add row using function in table.c
  add_row(table, row);

  // Write to table
  write_table(table);

  // TODO: Append to table instead of overwriting. Not working.
  // char * tablefilename = calloc(MAXIMUM_CHAR_COUNT_TABLE_NAME+8, sizeof(char));
  // strncpy(tablefilename, table->name, MAXIMUM_CHAR_COUNT_TABLE_NAME);
  // strcat(tablefilename, ".tbl");
  
  // int fd = open(tablefilename, O_WRONLY | O_APPEND);
  // if(fd == -1){
  //   printf("Error when attempting to open '%s' for writing, exiting: %s\n", tablefilename, strerror(errno));
  //   exit(1);
  // }
  // ssize_t res = write(fd, row->values, sizeof(row->values));
  // if (res == -1) {
  //   printf("Error when writing to table: %s\n", strerror(errno));
  //   exit(EXIT_FAILURE);
  // }
  // free(tablefilename);
  // close(fd);
  // struct table * tmp_table = read_table(table->name);
  // print_table(tmp_table);

  printf("Row added successfully to table '%s'!\n", table->name);

  return 0;
}

int add_col_cmd(struct table * table, char *args) {
  // Add column
  table->colcount++;
  if (table->colcount > MAXIMUM_COL_COUNT) {
    printf("Error: Column limit exceeded!\n");
    exit(EXIT_FAILURE);
  }
  strcpy(table->columnnames[table->colcount - 1], args);

  // Fill in new column
  printf("Fill each element in the new column:\n");
  for (int i = 0; i < table->rowcount; i++) {
    // Resize row
    struct intvector * currow = (table->data->values)[i]; 
    resize_intvector(currow, table->colcount);

    // Print row
    printf("Row %d: ", i);
    for (int j = 0; j < table->colcount - 1; j++) {
      printf("%d ", currow->values[j]); 
    }

    // User input
    char input[10];
    fgets(input, 10, stdin);
    chop_newline(input);
    int new_val = 0;
    sscanf(input, "%d", &new_val);

    // Add to vector
    add_intvector(currow, new_val);
  }

  // Write to table
  if (write_table(table) == 0) {
    printf("Column added successfully to table '%s'!\n", table->name);
  }
  else {
    printf("Error when writing to table: %s\n", strerror(errno));
    exit(EXIT_FAILURE);
  }

  return 0;
}

void table_parser(struct table * table) {
  // Table variables for ease of access 
  char *table_name = table->name;

  // Prompt for user input
  char *input_str = malloc(MAX_CMD_LENGTH);
  printf("Opened table '%s'. Input table command:\n", table_name);
  fgets(input_str, MAX_CMD_LENGTH, stdin);
  chop_newline(input_str);

  char *cmd = strsep(&input_str, " ");

  // Router for commands not requiring an argument
  if (!strcmp(cmd, "PRINT")) {
    print_table(table);
    return;
  }
  else if (!strcmp(cmd, "SORT")) { // TODO: Implement SORT. Warning: advanced feature! 
    return;
  }
  else if (!input_str) {
    printf("Error: argument not supplied!\n");
    exit(EXIT_FAILURE);
  }
  
  // Router for commands requiring an argument
  if (!strcmp(cmd, "ADDROW")) {
    add_row_cmd(table, input_str);
  }
  else if (!strcmp(cmd, "DELROW")) { // TODO: Implement DELROW

  }
  else if (!strcmp(cmd, "SETROW")) { // TODO: Implement SETROW

  }
  else if (!strcmp(cmd, "UPDATE")) { // TODO: Implement UPDATE

  }
  else if (!strcmp(cmd, "ADDCOL")) { // TODO: Implement ADDCOL
    add_col_cmd(table, input_str);
  }
  else if (!strcmp(cmd, "DELCOL")) { // TODO: Implement DELCOL

  }
  else if (!strcmp(cmd, "QUERY")) { // TODO: Implement QUERY

  }
  else {
    // TODO: Ask user to try again instead, within SELECT
    printf("Invalid command '%s'!\n", cmd);
    exit(EXIT_FAILURE);
  }
}

int select_table(char *args) {
  // Process args and put together file path
  char *table_name = strsep(&args, " ");
  char *file_dir = "./db/";
  char *file = malloc(sizeof(table_name) + sizeof(file_dir) + 1);
  strcpy(file, file_dir);
  file[strlen(file)] = '\0';
  file = strcat(file, table_name);
  // printf("%s\n", file);

  // Open table
  struct table * table = read_table(table_name);
  table_parser(table);
  
  // TODO: response after table_parser
  return 0;
}

int create_table(char *args) {
  // Create database directory if not exist
  char *file_dir = "./db/";
  struct stat st = {0};
  if (stat(file_dir, &st) == -1) {
    mkdir(file_dir, 0700);
  }

  // Process args and put together file path
  char *table_name = strsep(&args, " ");
  char *file = malloc(sizeof(table_name) + sizeof(file_dir) + 1);
  strcpy(file, file_dir);
  file = strcat(file, table_name);
  // printf("%s\n", file);

  // 2D array for column names. See tabledebug.c
  char *col_names[MAXIMUM_COL_COUNT];
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
  char *file_dir = "./db/";
  char *file_ext = ".tbl";
  char *file = malloc(sizeof(table_name) + sizeof(file_dir) + sizeof(file_ext) + 1);
  strcpy(file, file_dir);
  strcat(file, table_name);
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

int global_parser(char *input) {
  // Create a copy of input string
  char *input_str = malloc(sizeof(input));
  strcpy(input_str, input);

  // Get the command
  char *cmd = strsep(&input_str, " ");

  if (!strcmp(cmd, "EXIT")) {
    free(cmd);
    return -1;
  }

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

  return 0;
}