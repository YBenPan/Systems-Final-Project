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

  sleep(3); // To test semaphores

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
  free(row);
  free(row_item);

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
  if (key == -1) {
    printf("Error: Semaphore key of table '%s' not found in the key-name pairing file!\n", table_name);
    exit(EXIT_FAILURE);
  }
  
  while (1) {
    // Prompt for user input
    printf("Input table command:\n");
    fgets(input, MAX_CMD_LENGTH, stdin);
    chop_newline(input);
    
    int r = table_parser(table_name, input, key);
    if (r == -1) {
      printf("Table connection exiting. Back to global shell...\n\n");
      break;
    }
  }
  free(input);
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
    exit(EXIT_FAILURE);
  } 

  char *cmd = strsep(&input, " ");
  // Router for commands not requiring an argument
  if (!strcmp(cmd, "EXIT")) {
    // free(cmd);
    return -1;
  }
  else if (!strcmp(cmd, "PRINT")) {
    table = read_table(table_name);
    print_table(table);
    printf("\n");
    
    // Up by 1
    sb.sem_op = 1;
    if (semop(semd, &sb, 1) == -1) {
      printf("Error when performing an atomic operation: %s\n", strerror(errno));
      exit(EXIT_FAILURE);
    } 
    return 0;
  }
  else if (!input) {
    printf("Error: argument not supplied!\n\n");
    exit(EXIT_FAILURE);
  }

  // Down by SEM_MAX - 1
  sb.sem_op = -(_POSIX_SEM_VALUE_MAX - 1);
  if (semop(semd, &sb, 1) == -1) {
    printf("Error when performing an atomic operation: %s\n", strerror(errno));
    exit(EXIT_FAILURE);
  }
  table = read_table(table_name);
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
  else if (!strcmp(cmd, "SORT")) { // TODO: Implement SORT. Warning: advanced feature! 
    return 0;
  }
  else {
    // TODO: Ask user to try again instead, within SELECT
    printf("Invalid command '%s'!\n\n", cmd);
    exit(EXIT_FAILURE);
  }

  sb.sem_op = _POSIX_SEM_VALUE_MAX;
  if (semop(semd, &sb, 1) == -1) {
    printf("Error when performing an atomic operation: %s\n", strerror(errno));
    exit(EXIT_FAILURE);
  }

  // v = semctl(semd, 0, GETVAL, 0);
  // printf("%d\n", v);

  free(table);
  
  return 0;
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
    // free(table);
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
    exit(EXIT_FAILURE);
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