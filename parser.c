#include <assert.h>
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
#include "table.h"
#include "vector.h"
#include "file_io.h"
#include "schema.h"
#include "datatypes.h"
#include "networking_helper.h"
#include "strcmds.h"

union semun {
    int              val;    /* Value for SETVAL */
    struct semid_ds *buf;    /* Buffer for IPC_STAT, IPC_SET */
    unsigned short  *array;  /* Array for GETALL, SETALL */
    struct seminfo  *__buf;  /* Buffer for IPC_INFO */
                             /* (Linux-specific) */
  };

int add_row_cmd(struct table * table, char *args) {  
  //printf("table data size: %d\n", table->data->size);
  // Check syntax "(...)" and remove parentheses
  if (args[0] != '(' || args[strlen(args) - 1] != ')') {
    printf("Syntax Error: Row must be surrounded by parentheses!\n\n");
    return 1;
  }
  strsep(&args, "(");
  args[strlen(args) - 1] = '\0';
  
  // Form vector
  struct vector *row = init_vector();
  char *row_item; 

  // Deconstruct user input into row elements
  while ((row_item = strsep(&args, " "))) {
    //sscanf(row_item, "%d", &row_num);
    char *newbuff = calloc(strlen(row_item) + 1, sizeof(char));
    strncpy(newbuff, row_item, strlen(row_item) + 1);
    add_vector(row, newbuff); 
  }

  if(row->size != table->colcount){
    printf("Syntax Error: Row must have colcount = %d elements, found %d elements instead, exiting!\n\n", table->colcount, row->size);
    free_vector(row);
    return 1;
  }

  // Add row using function in table.c
  char retval = add_row_from_text(table, table->schm, (char **)row->values);
  free_vector(row);

  if(retval){
    printf("Encountered error in add_row_from_text when trying to add_row, exiting!\n\n");
    return 1;
  }

  // Add row using function in table.c
  //add_row(table, row);

  // Write to table
  write_table(table);

  sleep(3); // To test semaphores

  // TODO: Append to table instead of overwriting. Not working.
  // char * tablefilename = calloc(MAXIMUM_CHAR_COUNT_TABLE_NAME+8, sizeof(char));
  // strncpy(tablefilename, table->name, MAXIMUM_CHAR_COUNT_TABLE_NAME);
  // strcat(tablefilename, ".tbl");
  
  // int fd = open(tablefilename, O_WRONLY | O_APPEND);
  // if(fd == -1){
  //   printf("Error when attempting to open '%s' for writing, exiting: %s\n", tablefilename, strerror(errno));
  //   return 0;
  // }
  // ssize_t res = write(fd, row->values, sizeof(row->values));
  // if (res == -1) {
  //   printf("Error when writing to table: %s\n", strerror(errno));
  //   return 0;
  // }
  // free(tablefilename);
  // close(fd);
  // struct table * tmp_table = read_table(table->name);
  // print_table(tmp_table);

  printf("Row added successfully to table '%s'!\n\n", table->name);

  return 0;
}

int del_row_cmd(struct table * table, char *args) {
  // Parse row_index
  int row_index = 0;
  if (sscanf(args, "%d", &row_index) < 1) {
    printf("Error: invalid row index. Exiting!\n");
    return 1;
  }
  
  // Verify row_index falls in the range
  if (row_index < 0 || row_index >= table->rowcount) {
    printf("Error: Row index outside of the range of the table rows. Exiting!\n");
    return 1;
  }

  // Delete row by row index
  delete_vector(table->data, row_index);
  table->rowcount--;
  printf("Row %d successfully deleted! Updated table: \n", row_index);
  print_table(table);
  printf("\n");

  // Write to file
  write_table(table);

  return 0;
}

int set_row_cmd(struct table * table, char *args) {
  // Parse row_index
  int row_index = 0;
  char *row_index_str = strsep(&args, " "); 
  if (sscanf(row_index_str, "%d", &row_index) < 1) {
    printf("Error: invalid row index. Exiting!\n");
    return 1;
  }

  // Verify row_index falls in the range and args is correct
  if (row_index < 0 || row_index >= table->rowcount) {
    printf("Error: Row index outside of the range of the table rows. Exiting!\n");
    return 1;
  }
  if (!args) { 
    printf("Error: invalid row to be updated. Exiting!\n");
    return 1;
  }

  // Add row to table first
  if (add_row_cmd(table, args)) {
    printf("Error: adding row to table failed. Exiting!\n");
    return 1;
  }

  // Set row by row index
  table->data->values[row_index] = table->data->values[--table->rowcount];
  free(table->data->values[table->rowcount]);
  printf("Row %d successfully set! Updated table: \n", row_index);
  print_table(table);
  printf("\n");

  // Write to file
  write_table(table);

  return 0;
}

int add_col_cmd(struct table * table, char *args) {
  // printf("table data size: %d\n", table->data->size);
  // Add column
  table->colcount++;
  if (table->colcount > MAXIMUM_COL_COUNT) {
    table->colcount--;
    printf("Error: Column limit exceeded!\n\n");
    return 1;
  }
  //strcpy(table->columnnames[table->colcount - 1], args);
  
  // don't actually add it yet so we can revert it
  table->colcount--;

  // Fill in new column (with default entry because otherwise this will become hell)

  printf("Input type of entries in the new column:\n");
  char input[MAX_CMD_LENGTH];
  //fgets(input, MAX_CMD_LENGTH, stdin);
  readMacro(input, MAX_CMD_LENGTH);
  chop_newline(input);

  //printf("table data size: %d\n", table->data->size);

  struct datatype * new_data_type = parse_string_to_datatype(input);
  if(!new_data_type){
    printf("ERROR: datatype %s invalid, program exiting!\n\n", input);
    return 1;
  }

  printf("Input default entry for each element in new column:\n");
  char input2[MAX_CMD_LENGTH];
  //fgets(input2, MAX_CMD_LENGTH, stdin);
  readMacro(input2, MAX_CMD_LENGTH);
  chop_newline(input2);

  char *new_data_buff = parse_string_to_data(input2, new_data_type);
  if(!new_data_buff){
    printf("ERROR: Default entry %s couldn't be parsed to datatype ", input2);
    print_datatype(new_data_type);
    printf(", program exiting!\n\n");
    return 1;
  }

  //printf("table data size: %d\n", table->data->size);

  // input validation done, now actually start changing values

  // step 1: increment col count:
  table->colcount++;
  table->schm->colcount++;

  // step 2: update column names
  char (*newcolnames)[MAXIMUM_COL_LENGTH] = realloc(table->columnnames, table->colcount * MAXIMUM_COL_LENGTH);
  if(!newcolnames){
    printf("ERROR in add_col_cmd: realloc returned null pointer, program exiting!\n\n");
    return 1;
  }

  //printf("table data size: %d\n", table->data->size);

  table->columnnames = newcolnames;
  strncpy(newcolnames[table->colcount - 1], args, MAXIMUM_COL_LENGTH - 1);
  table->columnnames[table->colcount - 1][MAXIMUM_COL_LENGTH - 1] = '\0';

  //printf("table data size: %d\n", table->data->size);

  // step 3: update schema
  //printf("NEW DATA TYPE: ");
  //print_datatype(new_data_type);
  //printf("\n");
  add_vector(table->schm->datatypes, new_data_type);
  //printf("DEBUG\n");
  recompute_rowbytesize(table->schm);
  //printf("DEBUG2\n");

  //printf("table data size: %d\n", table->data->size);

  // step 4: update table values
  for(int i = 0; i < table->rowcount; ++i){
    //printf("rowcount %d, i=%d \n", table->rowcount, i);
    //printf("DEBUG2\n");
    //printf("table data size: %d\n", table->data->size);
    struct tablerow * currow = (table->data->values)[i];
    //printf("DEBUG\n");
    //printf("old buff length: %d\n", table->schm->rowbytesize[table->colcount - 1]);
    //printf("new buff length: %d\n", table->schm->rowbytesize[table->colcount]);
    currow->data = realloc(currow->data, table->schm->rowbytesize[table->colcount]);
    memcpy(currow->data + table->schm->rowbytesize[table->colcount - 1], new_data_buff, get_datatype_size(new_data_type));
  }

/*
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
*/

  // Write to table
  if (write_table(table) == 0) {
    printf("Column added successfully to table '%s'!\n\n", table->name);
  }
  else {
    printf("Error when writing to table: %s\n\n", strerror(errno));
    return 1;
  }

  return 0;
}

int del_col_cmd(struct table * table, char *args) {
  // Parse col_index
  int col_index = 0;
  if (sscanf(args, "%d", &col_index) < 1) {
    printf("Error: invalid column index. Exiting!\n");
    return 1;
  }
  printf("%d\n", col_index);
  printf("DEBUG1\n");

  // TODO: FIND INPUT IN COLNAMES AND GET COL_INDEX
  
  // Verify col_index falls in the range
  if (col_index < 0 || col_index >= table->colcount) {
    printf("Error: Column index outside of the range of the table columns. Exiting!\n");
    return 1;
  }

  // Go through each row and delete the element belonging to the column
  for (int i = 0; i < table->rowcount; i++) {
    struct tablerow * currow = table->data->values[i];
    int offset = currow->schm->rowbytesize[col_index];
    int len = get_datatype_size(currow->schm->datatypes->values[col_index]);
    char *start = currow->data + offset;
    char *end = currow->data + offset + len;
    memmove(start, start + len, strlen(end) + 1); // DOES NOT WORK IF MOVING CHAR OR TEXT

    // free(&currow->schm->rowbytesize[currow->schm->colcount - 1]);
    // delete_vector(currow->schm->datatypes, col_index);

    // Delete from tablerow schema too 
    for (int j = col_index; j < currow->schm->colcount - 1; j++) {
      currow->schm->rowbytesize[j] = currow->schm->rowbytesize[j + 1];
    }
    currow->schm->colcount--;
  }

  printf("DEBUG2\n");
  // Delete column
  for (int i = col_index; i < table->colcount - 1; i++) {
    strcpy(table->columnnames[i], table->columnnames[i + 1]);
  }
  // free(table->columnnames[--table->colcount]);
  --table->colcount;
  printf("DEBUG3\n");
  
  // Write to table
  if (write_table(table) == 0) {
    printf("Column deleted successfully from table '%s'!\n\n", table->name);
  }
  else {
    printf("Error when writing to table: %s\n\n", strerror(errno));
    return 1;
  }

  printf("Updated table:\n");
  print_table(table);
  printf("\n");

  return 0;
}


void table_main(char *table_name) {
  char *input = malloc(MAX_CMD_LENGTH);
  printf("Opened table '%s'. Entering table-specific shell...\n\n", table_name);
  // Semaphores start here
  int vector_size = 0, key = -1;
  int fd = open("./sem", O_RDONLY);
  read(fd, &vector_size, sizeof(int));

  // Read vectors from file
  for (int i = 0; i < vector_size; i++) {
    int new_table_name_size, new_semaphore_key;
    read(fd, &new_table_name_size, sizeof(int));
    char *new_table_name = malloc(new_table_name_size);
    read(fd, new_table_name, new_table_name_size);
    read(fd, &new_semaphore_key, sizeof(int));
    if (!strcmp(table_name, new_table_name)) { // Found semaphore key
      key = new_semaphore_key;
      break;
    }
  }
  // if (key == -1) {
  //   printf("Error: Semaphore key of table '%s' not found in the key-name pairing file!\n", table_name);
  //   return;
  // }
  
  while (1) {
    // Prompt for user input
    printf("Input table command:\n");
    //fgets(input, MAX_CMD_LENGTH, stdin);
    readMacro(input, MAX_CMD_LENGTH);
    chop_newline(input);
    
    int r = table_parser(table_name, input, key);
    if (r == -1) {
      printf("Table connection exiting. Back to global shell...\n\n");
      break;
    }
  }
}

int table_parser(char *table_name, char *input, int key) {
  
  struct table * table;

  int semd = semget(key, 1, 0);
  // int v = semctl(semd, 0, GETVAL, 0);
  // printf("%d\n", v);

  struct sembuf sb;
  sb.sem_num = 0; 
  sb.sem_flg = SEM_UNDO;
  // Down by 1
  sb.sem_op = -1;
  if (semop(semd, &sb, 1) == -1) {
    printf("Error when performing an atomic operation: %s\n", strerror(errno));
    return 1;
  } 

  char *cmd = strsep(&input, " ");
  // Router for commands not requiring an argument
  if (!strcmp(cmd, "EXIT")) {
    // Up by 1
    sb.sem_op = 1;
    if (semop(semd, &sb, 1) == -1) {
      printf("Error when performing an atomic operation: %s\n", strerror(errno));
      return 1;
    } 
    return -1;
  }
  else if (!strcmp(cmd, "PRINT")) {
    if (!input) {
      printf("Error: Specify target of print: SCHEMA or TABLE!\n");
      
      // Up by 1
      sb.sem_op = 1;
      if (semop(semd, &sb, 1) == -1) {
        printf("Error when performing an atomic operation: %s\n", strerror(errno));
        return 1;
      } 
      return 1;
    }
    else if (!strcmp(input, "SCHEMA")) {
      table = read_table(table_name);
      print_schema(table->schm);
      printf("\n");

      // Up by 1
      sb.sem_op = 1;
      if (semop(semd, &sb, 1) == -1) {
        printf("Error when performing an atomic operation: %s\n", strerror(errno));
        return 1;
      } 
      return 0;
    }
    else if (!strcmp(input, "TABLE")) {
      table = read_table(table_name);
      print_table(table);
      printf("\n");
      
      // Up by 1
      sb.sem_op = 1;
      if (semop(semd, &sb, 1) == -1) {
        printf("Error when performing an atomic operation: %s\n", strerror(errno));
        return 1;
      } 
      return 0;
    }
    else {
      printf("Error: Unknown PRINT argument.\n");
      // Up by 1
      sb.sem_op = 1;
      if (semop(semd, &sb, 1) == -1) {
        printf("Error when performing an atomic operation: %s\n", strerror(errno));
        return 1;
      } 
      return 1;
    }
  }

  // Down by SEM_MAX - 1
  sb.sem_op = -(_POSIX_SEM_VALUE_MAX - 1);
  if (semop(semd, &sb, 1) == -1) {
    printf("Error when performing an atomic operation: %s\n", strerror(errno));
    return 1;
  }
  table = read_table(table_name);

  // Router for commands requiring an argument
  if (!strcmp(cmd, "ADDROW")) {
    checkInput(input);
    add_row_cmd(table, input);
  }
  else if (!strcmp(cmd, "DELROW")) {
    checkInput(input);
    del_row_cmd(table, input);
  }
  else if (!strcmp(cmd, "SETROW")) {
    checkInput(input);
    set_row_cmd(table, input);
  }
  // else if (!strcmp(cmd, "UPDATE")) { 
  //   checkInput(input);
  // }
  else if (!strcmp(cmd, "ADDCOL")) { // 
    checkInput(input);
    add_col_cmd(table, input);
  }
  else if (!strcmp(cmd, "DELCOL")) { // TODO: Implement DELCOL
    checkInput(input);
    printf("Only works with integer tables because of memmove issue. Proceed with caution!\n");
    del_col_cmd(table, input);
  }
  // else if (!strcmp(cmd, "QUERY")) { // TODO: Implement QUERY
  //   checkInput(input);
  // }
  // else if (!strcmp(cmd, "SORT")) {
  //   checkInput(input);
  //   return 0;
  // }
  else {
    printf("Invalid command '%s'!\n\n", cmd);
    sb.sem_op = _POSIX_SEM_VALUE_MAX;
    if (semop(semd, &sb, 1) == -1) {
      printf("Error when performing an atomic operation: %s\n", strerror(errno));
      return 1;
    }
    return 1;
  }

  sb.sem_op = _POSIX_SEM_VALUE_MAX;
  if (semop(semd, &sb, 1) == -1) {
    printf("Error when performing an atomic operation: %s\n", strerror(errno));
    return 1;
  }

  // v = semctl(semd, 0, GETVAL, 0);
  // printf("%d\n", v);

  free(table);

  return 0;
}

int select_table(char *args) {
  // Process args and put together file path
  char *table_name = strsep(&args, " ");
  //char *file = malloc(sizeof(table_name) + 16);
  //strcpy(file, "./db/");
  //strncat(file, table_name, MAXIMUM_CHAR_COUNT_TABLE_NAME);
  //strcat(file, ".tbl");
  // printf("%s\n", file);

  // Open table
  table_main(table_name);
  
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
  if(!table_name){
    printf("ERROR: Invalid table name! Exiting!\n");
    return 1;
  }
  // char *file = malloc(sizeof(table_name) + sizeof(file_dir) + 1);
  // strcpy(file, file_dir);
  // file = strcat(file, table_name);
  // printf("%s\n", file);

  // 2D array for column names. See tabledebug.c
  char *col_names[MAXIMUM_COL_COUNT];
  // table->column_names -> char (*col_names)[64] -> pointer (dynamic array) to 64-sized fixed-length char arrays
  char *col_input = malloc(MAX_CMD_LENGTH);
  printf("Input column names, separated by space:\n");
  //fgets(col_input, MAX_CMD_LENGTH, stdin);
  readMacro(col_input, MAX_CMD_LENGTH);
  //printf("DEBUG-1:\n");
  chop_newline(col_input);

  //printf("DEBUG0: col_input =%s\n", col_input);

  // Parse the column names
  int col_cnt = 0;
  while (1) {
    char *col_name = strsep(&col_input, " ");
    if (!col_name) {
      break;
    }
    col_names[col_cnt] = malloc((strlen(col_name) + 1) * sizeof(char));
    strcpy(col_names[col_cnt++], col_name);
    //printf("%s\n", col_name);
  }

  printf("Input schema (data types) for each column, separated by spaces. You must have as many data types as you have columns.\n");
  printf("Allowed data types include INT, SMALLINT, TINYINT, LONG, FLOAT, DOUBLE, CHAR, and TEXT(n), where n is an integer.\n");
  col_input = malloc(MAX_CMD_LENGTH);
  //fgets(col_input, MAX_CMD_LENGTH, stdin);
  //printf("DEBUG2\n");
  readMacro(col_input, MAX_CMD_LENGTH);
  //printf("DEBUG1: col_input =%s\n", col_input);
  chop_newline(col_input);
  //printf("DEBUG2: col_input =%s\n", col_input);
  //printf("DEBUG\n");
  struct schema * schema = init_schema_from_text(col_cnt, col_input);
  //printf("DEBUG3\n");
  if(!schema){
    printf("ERROR: When trying to create table, init_schema_from_text was unable to parse your schema string correctly. Program exiting!\n");
    free(col_input);
    for(int i = 0; i < col_cnt; ++i){
      free(col_names[i]);
    }
    return 1;
  }
  //printf("DEBUG4\n");
  // Initialize table and free col_names
  struct table * table = init_table(table_name, col_names, col_cnt, schema);
  //printf("DEBUG4.1\n");
  for(int i = 0; i < col_cnt; ++i){
    free(col_names[i]);
  }
  //printf("DEBUG5\n");
  // Write table
  if (!write_table(table)) {
    // free(table);
  }
  else {
    printf("Creation of table '%s' failed: %s\n\n", table_name, strerror(errno));
    return 1;
  }

  srand(time(NULL));

  // TODO: keep generating key until it's unique
  int key = rand() % 100000;

  // Create semaphore
  int semd = semget(key, 1, IPC_CREAT | IPC_EXCL | 0644);
  int v;
  if (semd == -1) {
      printf("Error: %s\n", strerror(errno));
      return 1;
  }

  // Set semaphore value
  union semun us;
  us.val = _POSIX_SEM_VALUE_MAX;
  v = semctl(semd, 0, SETVAL, us);
  if (v == -1) {
    printf("Error: %s\n", strerror(errno));
    return 1;
  }
  else printf("Created semaphore with key '%d' successfully!\n", key);

  // Open semaphore file
  int fd = open("./sem", O_RDONLY | O_CREAT, 0700);
  if (fd == -1) {
    printf("Error when opening semaphore file: %s\n", strerror(errno));
    return 1;
  }
  
  // Check if file is blank
  struct stat stat_record;
  if (fstat(fd, &stat_record) == -1) {
    printf("Error when checking semaphore file: %s\n", strerror(errno));
    return 1;
  }
  
  // Initialize vectors 
  struct vector * table_names;
  struct intvector * semaphore_keys;
  int vector_size = 0;

  if (stat_record.st_size < 1) {
    table_names = init_vector();
    semaphore_keys = init_intvector();
  }
  else { // Nonempty file
    read(fd, &vector_size, sizeof(int));
    table_names = init_vector();
    semaphore_keys = init_intvector();

    // Read vectors from file
    for (int i = 0; i < vector_size; i++) {
      int new_table_name_size, new_semaphore_key;
      read(fd, &new_table_name_size, sizeof(int));
      char *new_table_name = malloc(new_table_name_size);
      read(fd, new_table_name, new_table_name_size);
      read(fd, &new_semaphore_key, sizeof(int));
      add_vector(table_names, new_table_name);
      add_intvector(semaphore_keys, new_semaphore_key);
    }
    
  }
    
  // Add new table's name and semaphore key to the vectors
  add_vector(table_names, table_name);
  add_intvector(semaphore_keys, key);
  vector_size++;

  // printf("---\n");
  // printf("%d\n", vector_size);
  // for (int i = 0; i < vector_size; i++) {
  //   printf("%s: %d\n", table_names->values[i], semaphore_keys->values[i]);
  // }
  // printf("---\n");

  fd = open("./sem", O_WRONLY | O_TRUNC, 0700);

  // First integer identify the number of tables
  write(fd, &vector_size, sizeof(int));

  for (int i = 0; i < vector_size; i++) {
    int strl = strlen(table_names->values[i]);
    write(fd, &strl, sizeof(int)); // Length of table_name string
    write(fd, table_names->values[i], strl);
    write(fd, &semaphore_keys->values[i], sizeof(int));
  }

  printf("Semaphore keys file updated!\n");
  close(fd);
  printf("Table '%s' created successfully!\n\n", table_name);

  free(table_names);
  free(semaphore_keys);
  
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

  // Open table_name-semaphore_key pairing file
  int fd = open("./sem", O_RDONLY, 0700);
  struct vector * table_names;
  struct intvector * semaphore_keys;
  int vector_size = 0;

  read(fd, &vector_size, sizeof(int));
  // printf("Vector size: %d\n", vector_size);
  table_names = init_vector();
  semaphore_keys = init_intvector();

  // Read vectors from file
  int tmp = vector_size;
  for (int i = 0; i < tmp; i++) {
    int new_table_name_size, new_semaphore_key;
    read(fd, &new_table_name_size, sizeof(int));
    char *new_table_name = malloc(new_table_name_size);
    read(fd, new_table_name, new_table_name_size);
    read(fd, &new_semaphore_key, sizeof(int));

    // Skip the dropped table
    if (strcmp(table_name, new_table_name)) {
      add_vector(table_names, new_table_name);
      add_intvector(semaphore_keys, new_semaphore_key);
    }
    else {
      vector_size--;
    }
  }

  // printf("---\n");
  // printf("%d\n", vector_size);
  // for (int i = 0; i < vector_size; i++) {
  //   printf("%s: %d\n", table_names->values[i], semaphore_keys->values[i]);
  // }
  // printf("---\n");

  fd = open("./sem", O_WRONLY | O_TRUNC, 0700);

  // First integer identify the number of tables
  write(fd, &vector_size, sizeof(int));

  for (int i = 0; i < vector_size; i++) {
    int strl = strlen(table_names->values[i]);
    write(fd, &strl, sizeof(int)); // Length of table_name string
    write(fd, table_names->values[i], strl);
    write(fd, &semaphore_keys->values[i], sizeof(int));
  }

  printf("Semaphore keys file updated!\n");
  close(fd);

  if (remove(file) == -1) {
    printf("Removing table '%s' failed: %s\n\n", table_name, strerror(errno));
    return 1;
  }
  printf("Table '%s' dropped successfully!\n\n", table_name);

  return 0;
}

int global_parser(char *input) {

  // Get the command
  char *cmd = strsep(&input, " ");

  if (!strcmp(cmd, "EXIT")) {
    return -1;
  }
  if (!strcmp(cmd, "SELECT")) {
    checkInput(input);
    select_table(input);
  }
  else if (!strcmp(cmd, "CREATE")) {
    checkInput(input);
    create_table(input);
  }
  else if (!strcmp(cmd, "DROP")) {
    checkInput(input);
    drop_table(input);
  }
  else if (!strcmp(cmd, "TERMINATE")){
    exit(0);
  }
  else {
    printf("Invalid command '%s'!\n\n", cmd);
    return 1;
  }

  return 0;
}