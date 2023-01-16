#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <time.h>
#include <unistd.h>

#include "parser.h"

// union semun {
//    int              val;    /* Value for SETVAL */
//    struct semid_ds *buf;    /* Buffer for IPC_STAT, IPC_SET */
//    unsigned short  *array;  /* Array for GETALL, SETALL */
//    struct seminfo  *__buf;  /* Buffer for IPC_INFO */
//                             /* (Linux-specific) */
//  };

void chop_newline(char *s) {
  size_t ln = strlen(s) - 1;
  if (*s && s[ln] == '\n') 
    s[ln] = '\0';
}

int add_row_cmd(struct table * table, char *args) {  
  // Check syntax "(...)" and remove parentheses
  if (args[0] != '(' || args[strlen(args) - 1] != ')') {
    printf("Syntax Error: Row must be surrounded by parentheses!\n\n");
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

  printf("Row added successfully to table '%s'!\n\n", table->name);

  return 0;
}

int add_col_cmd(struct table * table, char *args) {
  // Add column
  table->colcount++;
  if (table->colcount > MAXIMUM_COL_COUNT) {
    printf("Error: Column limit exceeded!\n\n");
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
    printf("Column added successfully to table '%s'!\n\n", table->name);
  }
  else {
    printf("Error when writing to table: %s\n\n", strerror(errno));
    exit(EXIT_FAILURE);
  }

  return 0;
}

void table_main(struct table * table) {
  char *input = malloc(MAX_CMD_LENGTH);
  printf("Opened table '%s'. Entering table-specific shell...\n\n", table->name);
  
  while (1) {
    // Prompt for user input
    printf("Input table command:\n");
    fgets(input, MAX_CMD_LENGTH, stdin);
    chop_newline(input);

    // TODO: semaphore goes here

    int r = table_parser(table, input);
    if (r == -1) {
      printf("Table connection exiting. Back to global shell...\n\n");
      break;
    }
  }
}

int table_parser(struct table * table, char *input) {

  // Get the command
  char *cmd = strsep(&input, " ");

  // Router for commands not requiring an argument
  if (!strcmp(cmd, "EXIT")) {
    free(cmd);
    return -1;
  }
  else if (!strcmp(cmd, "PRINT")) {
    print_table(table);
    printf("\n");
    return 0;
  }
  else if (!strcmp(cmd, "SORT")) { // TODO: Implement SORT. Warning: advanced feature! 
    return 0;
  }
  else if (!input) {
    printf("Error: argument not supplied!\n\n");
    exit(EXIT_FAILURE);
  }
  
  // Router for commands requiring an argument
  if (!strcmp(cmd, "ADDROW")) {
    add_row_cmd(table, input);
  }
  else if (!strcmp(cmd, "DELROW")) { // TODO: Implement DELROW

  }
  else if (!strcmp(cmd, "SETROW")) { // TODO: Implement SETROW

  }
  else if (!strcmp(cmd, "UPDATE")) { // TODO: Implement UPDATE

  }
  else if (!strcmp(cmd, "ADDCOL")) { // TODO: Implement ADDCOL
    add_col_cmd(table, input);
  }
  else if (!strcmp(cmd, "DELCOL")) { // TODO: Implement DELCOL

  }
  else if (!strcmp(cmd, "QUERY")) { // TODO: Implement QUERY

  }
  else {
    // TODO: Ask user to try again instead, within SELECT
    printf("Invalid command '%s'!\n\n", cmd);
    exit(EXIT_FAILURE);
  }
}

int select_table(char *args) {
  // Process args and put together file path
  char *table_name = strsep(&args, " ");
  char *file = malloc(sizeof(table_name) + 16);
  strcpy(file, "./db/");
  strncat(file, table_name, MAXIMUM_CHAR_COUNT_TABLE_NAME);
  strcat(file, ".tbl");
  // printf("%s\n", file);

  // Open table
  struct table * table = read_table(table_name);
  table_main(table);
  
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
  // char *file = malloc(sizeof(table_name) + sizeof(file_dir) + 1);
  // strcpy(file, file_dir);
  // file = strcat(file, table_name);
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
  }
  else {
    printf("Creation of table '%s' failed: %s\n\n", table_name, strerror(errno));
    exit(EXIT_FAILURE);
  }

  srand(time(NULL));

  // TODO: keep generating key until it's unique
  int key = rand() % 100000;

  // Create semaphore
  int semd = semget(key, 1, IPC_CREAT | IPC_EXCL | 0644);
  int v;
  if (semd == -1) {
      printf("Error: %s\n", strerror(errno));
      exit(EXIT_FAILURE);
  }

  // Set semaphore value
  union semun us;
  us.val = _POSIX_SEM_VALUE_MAX;
  v = semctl(semd, 0, SETVAL, us);
  if (v == -1) {
    printf("Error: %s\n", strerror(errno));
    exit(EXIT_FAILURE);
  }
  else printf("Created semaphore with key '%d' successfully!\n", key);

  // Open semaphore file
  int fd = open("./sem", O_RDONLY | O_CREAT, 0700);
  if (fd == -1) {
    printf("Error when opening semaphore file: %s\n", strerror(errno));
    exit(EXIT_FAILURE);
  }
  
  // Check if file is blank
  struct stat stat_record;
  if (fstat(fd, &stat_record) == -1) {
    printf("Error when checking semaphore file: %s\n", strerror(errno));
    exit(EXIT_FAILURE);
  }

  // Initialize vectors 
  struct vector * table_names = init_vector();
  struct vector * semaphore_keys = init_vector();
  int table_names_size = 0, semaphore_keys_size = 0;

  if (stat_record.st_size > 1) { // Nonempty file
    read(fd, &table_names_size, sizeof(int));
    read(fd, &semaphore_keys_size, sizeof(int));
    printf("%d, %d\n", table_names_size, semaphore_keys_size);
    if (table_names_size != 0 && semaphore_keys_size != 0) {
      // Read vectors from file
      read(fd, &table_names, table_names_size);
      read(fd, &semaphore_keys, semaphore_keys_size);
    }
  }
    
  // Add new table's name and semaphore key to the vectors
  add_vector(table_names, table_name);
  add_vector(semaphore_keys, &key);
  printf("%d\n", table_names->size);
  // for (int i = 0; i < table_names->size; i++) {
  //   printf("%s\n", table_names->values[i]);
  // }

  fd = open("./sem", O_WRONLY | O_TRUNC, 0700);

  // First two integers identify the sizes of vectors
  table_names_size = sizeof(table_names);
  semaphore_keys_size = sizeof(semaphore_keys);
  write(fd, &table_names_size, sizeof(int));
  write(fd, &semaphore_keys_size, sizeof(int));

  // Write the vectors to file
  write(fd, &table_names, table_names_size);
  write(fd, &semaphore_keys, semaphore_keys_size);
  printf("Semaphore keys file updated!\n");

  close(fd);

  printf("Table '%s' created successfully!\n\n", table_name);
  
  return 0;
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
    printf("Dropping table '%s' failed: %s\n\n", table_name, strerror(errno));
    exit(EXIT_FAILURE);
  }
  else {
    printf("Table '%s' dropped successfully!\n\n", table_name);
    return 0;
  }
}

int global_parser(char *input) {

  // Get the command
  char *cmd = strsep(&input, " ");

  if (!strcmp(cmd, "EXIT")) {
    free(cmd);
    return -1;
  }

  if (!strcmp(cmd, "SELECT")) {
    select_table(input);
  }
  else if (!strcmp(cmd, "CREATE")) {
    create_table(input);
  }
  else if (!strcmp(cmd, "DROP")) {
    drop_table(input);
  }
  else {
    printf("Invalid command '%s'!\n\n", cmd);
    // TODO: Ask user to try again instead
    exit(EXIT_FAILURE);
  }

  return 0;
}