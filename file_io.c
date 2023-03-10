#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>

#include "table.h"
#include "file_io.h"
#include "vector.h"
#include "schema.h"
#include "datatypes.h"

int get_alignment_padding(int raw_byte_count){
  return (TABLE_FILE_BINARY_ROW_ALIGNMENT - (raw_byte_count % TABLE_FILE_BINARY_ROW_ALIGNMENT)) % TABLE_FILE_BINARY_ROW_ALIGNMENT;
}

// returns 0 if success
char write_table(struct table * table){
  // Put together table path
  char * tablefilename = calloc(MAXIMUM_CHAR_COUNT_TABLE_NAME+16, sizeof(char));
  strcpy(tablefilename, "./db/");
  strncat(tablefilename, table->name, MAXIMUM_CHAR_COUNT_TABLE_NAME);
  strcat(tablefilename, ".tbl");
  int fd = open(tablefilename, O_CREAT | O_TRUNC | O_WRONLY, 0644);
  if(fd == -1){
    printf("Error when attempting to open the table file for writing, exiting: %s\n", strerror(errno));
    return 0;
  }
  int file_version = CURRENT_TABLE_FILE_VERSION;
  // HEADER
  write(fd, "TBLF", 4);
  write(fd, &file_version, sizeof(int)); // REMEMBER TO UPDATE THIS WHEN YOU CHANGE FILE FORMAT
  write(fd, &(table->colcount), sizeof(int));
  write(fd, &(table->rowcount), sizeof(int));
  // SECTION 1: COLUMN IDENTIFIERS
  for(int i = 0; i < table->colcount; ++i){
    write(fd, table->columnnames[i], MAXIMUM_COL_LENGTH);
  }
  // SECTION 2: SCHEMA
  for(int i = 0; i < table->colcount; ++i){
    struct datatype * datatype = table->schm->datatypes->values[i];
    write(fd, datatype, sizeof(struct datatype));
  }
  // SECTION 3: TABLE DATA
  char nullseg[] = "\0\0\0\0\0\0\0\0";
  for(int i = 0; i < table->rowcount; ++i){
    struct tablerow * currow = (table->data->values)[i];
    int rs = (table->schm->rowbytesize[table->schm->colcount]);
    write(fd, currow->data, rs);
    write(fd, nullseg, get_alignment_padding(rs));
  }
  free(tablefilename);
  close(fd);
  return 0;
}

struct table * read_table(char * table_name){
  // Put together table path
  char * tablefilename = calloc(MAXIMUM_CHAR_COUNT_TABLE_NAME + 16, sizeof(char));
  strcpy(tablefilename, "./db/");
  strncat(tablefilename, table_name, MAXIMUM_CHAR_COUNT_TABLE_NAME);
  strcat(tablefilename, ".tbl");
  int fd = open(tablefilename, O_RDONLY);
  if(fd == -1){
    printf("Error when attempting to open the table file for reading, exiting: %s\n", strerror(errno));
    return 0;
  }
  char nonce[5];
  read(fd, nonce, 4);
  nonce[4] = '\0';
  if(strncmp(nonce, "TBLF", 4)){
    printf("ERROR: Encountered invalid table file that does not begin with nonce TBLF, exiting!\n");
    return 0;
  }
  int file_version;
  read(fd, &file_version, sizeof(int));
  int row_count;
  int col_count;
  read(fd, &col_count, sizeof(int));
  read(fd, &row_count, sizeof(int));
  struct table * table = NULL;
  if(file_version == 1){
    // file version 1 process code
    char **columnnames = calloc(col_count, sizeof(char *));
    for(int i = 0; i < col_count; ++i){
      char *curname = calloc(MAXIMUM_CHAR_COUNT_TABLE_NAME, sizeof(char));
      read(fd, curname, MAXIMUM_CHAR_COUNT_TABLE_NAME);
      columnnames[i] = curname;
    }
    struct vector * datatypes = init_vector();
    for(int i = 0; i < col_count; ++i){
      struct datatype * dt = malloc(sizeof(struct datatype));
      read(fd, dt, sizeof(struct datatype));
      add_vector(datatypes, dt);
    }
    struct schema * schema = init_schema(col_count, datatypes);
    table = init_table(table_name, columnnames, col_count, schema);
    for(int i = 0; i < col_count; ++i){
      free(columnnames[i]);
    }
    free(columnnames);
    for(int i = 0; i < row_count; ++i){
      int rs = schema->rowbytesize[col_count];
      char * rowbuff = calloc(schema->rowbytesize[col_count], sizeof(char));
      read(fd, rowbuff, rs);
      lseek(fd, get_alignment_padding(rs), SEEK_CUR);
      struct tablerow * tblrow = malloc(sizeof(struct tablerow));
      tblrow->schm = schema;
      tblrow->data = rowbuff;
      add_row(table, tblrow);
    }
  } else if(file_version == 0){
    // file version 0 process code
    struct vector * datatypes = init_vector();
    for(int i = 0; i < col_count; ++i){
      add_vector(datatypes, parse_string_to_datatype("int"));
    }
    struct schema * schema = init_schema(col_count, datatypes);
    char **columnnames = calloc(col_count, sizeof(char *));
    for(int i = 0; i < col_count; ++i){
      char *curname = calloc(MAXIMUM_CHAR_COUNT_TABLE_NAME, sizeof(char));
      read(fd, curname, MAXIMUM_CHAR_COUNT_TABLE_NAME);
      columnnames[i] = curname;
    }
    table = init_table(table_name, columnnames, col_count, schema);
    //table->rowcount = row_count;
    for(int i = 0; i < col_count; ++i){
      free(columnnames[i]);
    }
    free(columnnames);
    for(int i = 0; i < row_count; ++i){
      int * rowbuff = calloc(col_count, sizeof(int));
      read(fd, rowbuff, sizeof(int) * col_count);
      //printf("test input %d\n", i);
      //struct intvector * rowvec = init_intvector();
      //resize_intvector(rowvec, col_count);
      //memcpy(rowvec->values, rowbuff, sizeof(int) * col_count);
/*
      for(int i = 0; i < col_count; ++i){
        printf("rowbuff %d: %d\n", i, rowbuff[i]);
      }
      for(int i = 0; i < col_count; ++i){
        printf("rowvec->values %d: %d\n", i, rowvec->values[i]);
      }
*/
      //rowvec->size = col_count;
      struct tablerow * tblrow = malloc(sizeof(struct tablerow));
      tblrow->schm = schema;
      tblrow->data = (char *)rowbuff;
      add_row(table, tblrow);
    }
  } else {
    printf("ERROR: Encountered non-supported table file version %d, exiting!\n\n", file_version);
    return 0;
  }
  free(tablefilename);
  close(fd);
  return table;
}

char * csv_ify(char *term){
  char *buff = calloc(2 * strlen(term) + 10, sizeof(char));
  buff[0] = CSV_ESCAPE_CHARACTER;
  char *curpos = buff + 1;
  //printf("DEBUG 1a\n");
  while(*term){
    if(*term == CSV_ESCAPE_CHARACTER){
      *curpos = CSV_ESCAPE_CHARACTER;
      curpos++;
    }
    *curpos = *term;
    curpos++;
    term++;
  }
  *curpos = CSV_ESCAPE_CHARACTER;
  curpos++;
  *curpos = '\0';
  //printf("DEBUG 1b\n");
  return buff;
}

// returns 0 if successful
char write_table_to_csv(struct table * table, char * output_file){
  int fd = open(output_file, O_CREAT | O_TRUNC | O_WRONLY, 0644);
  if(fd == -1){
    printf("Error when attempting to open the table file for writing, exiting: %s\n\n", strerror(errno));
    return 0;
  }
  char delim_char = CSV_DELIMITER_CHARACTER;
  char newline_char = '\n';
  //printf("DEBUG 1\n");
  for(int i = 0; i < table->colcount; ++i){
    if(i != 0){
      write(fd, &delim_char, sizeof(char));
    }
    char *csvifed_name = csv_ify(table->columnnames[i]);
    write(fd, csvifed_name, strlen(csvifed_name) * sizeof(char));
    free(csvifed_name);
  }
  //printf("DEBUG 2\n");
  write(fd, &newline_char, sizeof(char));
  for(int i = 0; i < table->rowcount; ++i){
    struct tablerow *tablerow = (table->data->values)[i];
    for(int j = 0; j < table->colcount; ++j){
      if(j != 0){
        write(fd, &delim_char, sizeof(char));
      }
      char * outputbuff = print_element_from_datatype_to_string(tablerow->data + tablerow->schm->rowbytesize[j], tablerow->schm->datatypes->values[j]);
      char * csvified_buff = csv_ify(outputbuff);
      dprintf(fd, "%s", csvified_buff);
      free(outputbuff);
      free(csvified_buff);
      //int cur = currow->values[j];
      //dprintf(fd, "%d", cur);
    }
    write(fd, &newline_char, sizeof(char));
  }
  close(fd);
  return 0;
}
