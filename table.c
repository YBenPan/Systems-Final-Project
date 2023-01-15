#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "table.h"
#include "vector.h"
#include "schema.h"
#include "datatypes.h"

struct table * init_table(char * tablename, char **columnnames, int colcount, struct schema * schm){
  if(schm->colcount != colcount){
    printf("ERROR: init_table called with colcount %d, expected colcount to match schema's %d, exiting!\n", colcount, schm->colcount);
    exit(1);
  }
  //printf("DEBUG T0\n");
  struct table * o = calloc(1, sizeof(struct table));
  o->schm = schm;
  //printf("DEBUG T1\n");
  strncpy(o->name, tablename, MAXIMUM_CHAR_COUNT_TABLE_NAME-1);
  //printf("DEBUG T2\n");
  o->colcount = colcount;
  o->rowcount = 0;
  o->columnnames = calloc(colcount, sizeof(char[MAXIMUM_CHAR_COUNT_TABLE_NAME]));
  for(int i = 0; i < colcount; ++i){
    //printf("%s\n", columnnames[i]);
    strncpy(o->columnnames[i], columnnames[i], MAXIMUM_CHAR_COUNT_TABLE_NAME-1);
  }
  //printf("DEBUG T3\n");
  o->data = init_vector();
  //printf("DEBUG T4\n");
  return o;
}

void add_row(struct table * table, struct tablerow * row){
  if(row->schm != table->schm){
    printf("ERROR: Tried to add row to table with schema pointer %p, expected pointer %p, program exiting!\n", row->schm, table->schm);
    exit(1);
  }
/*
  if(row->size != table->colcount){
    printf("ERROR: Tried to add row to table with size %d, expected colcount %d, program exiting.\n", row->size, table->colcount);
    exit(1);
  }
*/
  add_vector(table->data, row);
  table->rowcount++;
}

// text is 2d array of the same size as schm->colcount, i-th entry corresponds to i-th entry of schema
void add_row_from_text(struct table * table, struct schema * schm, char ** text){
  struct tablerow * newrow = malloc(sizeof(struct tablerow));
  tablerow->schm = schm;
  tablerow->data = calloc(schm->rowbytesize[schm->colcount], sizeof(char));
}

void print_table(struct table * table){
  printf("Columns: ");
  for(int i = 0; i < table->colcount; i++){
    if(i != 0){
      printf(", ");
    }
    printf("%s", table->columnnames[i]);
  }
  printf("\n");
  for(int i = 0; i < table->rowcount; i++){
    //printf("PRINT DEBUG %d\n", i);
    printf("Row %d: ", i);
    print_table_row((table->data->values)[i]);
    /*
    struct intvector * currow = (table->data->values)[i];
    for(int j = 0; j < table->colcount; j++){
      if(j != 0){
        printf(" ");
      }
      printf("%d",(currow->values[j]));
    }
    printf("]\n");
    */
    printf("\n");
  }
}

void print_table_row(struct tablerow * tablerow){
  printf("[");
  for(int i = 0; i < tablerow->schm->colcount; ++i){
    print_element_from_datatype(tablerow->data + tablerow->schm->rowbytesize[i], tablerow->schm->datatypes[i]);
  }
  printf("]");
}
