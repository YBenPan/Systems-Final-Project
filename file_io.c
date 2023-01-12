#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>

#include "table.h"
#include "file_io.h"
#include "vector.h"

// returns 0 if success
char write_table(struct table * table){
  char * tablefilename = calloc(72, sizeof(char));
  strncpy(tablefilename, table->name, 64);
  strcat(tablefilename, ".tbl");
  int fd = open(tablefilename, O_CREAT | O_TRUNC | O_WRONLY, 0644);
  if(fd == -1){
    printf("Error when attempting to open the table file for writing, exiting: %s\n", strerror(errno));
    exit(1);
  }
  int file_version = CURRENT_TABLE_FILE_VERSION;
  // HEADER
  write(fd, "TBLF", 4);
  write(fd, &file_version, sizeof(int)); // REMEMBER TO UPDATE THIS WHEN YOU CHANGE FILE FORMAT
  write(fd, &(table->colcount), sizeof(int));
  write(fd, &(table->rowcount), sizeof(int));
  // SECTION 1: COLUMN IDENTIFIERS
  for(int i = 0; i < table->colcount; ++i){
    write(fd, table->columnnames[i], 64);
  }
  // SECTION 2: TABLE DATA
  for(int i = 0; i < table->rowcount; ++i){
    struct intvector * currow = (table->data->values)[i];
    // table->colcount must be intvector->size, checked by add_row
    write(fd, currow->values, sizeof(int) * table->colcount);
  }
  free(tablefilename);
  close(fd);
  return 0;
}

struct table * read_table(char * table_name){
  char * tablefilename = calloc(72, sizeof(char));
  strncpy(tablefilename, table_name, 64);
  strcat(tablefilename, ".tbl");
  int fd = open(tablefilename, O_RDONLY);
  if(fd == -1){
    printf("Error when attempting to open the table file for reading, exiting: %s\n", strerror(errno));
    exit(1);
  }
  char nonce[5];
  read(fd, nonce, 4);
  nonce[4] = '\0';
  if(strncmp(nonce, "TBLF", 4)){
    printf("ERROR: Encountered invalid table file that does not begin with nonce TBLF, exiting!\n");
    exit(1);
  }
  int file_version;
  read(fd, &file_version, sizeof(int));
  int row_count;
  int col_count;
  read(fd, &col_count, sizeof(int));
  read(fd, &row_count, sizeof(int));
  struct table * table = NULL;
  if(file_version == 0){
    // file version 0 process code
    char **columnnames = calloc(col_count, sizeof(char *));
    for(int i = 0; i < col_count; ++i){
      char *curname = calloc(64, sizeof(char));
      read(fd, curname, 64);
      columnnames[i] = curname;
    }
    table = init_table(table_name, columnnames, col_count);
    table->rowcount = row_count;
    for(int i = 0; i < col_count; ++i){
      free(columnnames[i]);
    }
    free(columnnames);
    int * rowbuff = calloc(col_count, sizeof(int));
    for(int i = 0; i < row_count; ++i){
      read(fd, rowbuff, sizeof(int) * col_count);
      struct intvector * rowvec = init_intvector();
      resize_intvector(rowvec, col_count);
      memcpy(rowvec->values, rowbuff, sizeof(int) * col_count);
    }
    free(rowbuff);
  } else {
    printf("ERROR: Encountered non-supported table file version %d, exiting!\n", file_version);
    exit(1);
  }
  free(tablefilename);
  close(fd);
  return table;
}
