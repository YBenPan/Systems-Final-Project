#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "table.h"
#include "vector.h"

struct table * init_table(char * tablename, char **columnnames, int colcount){
  //printf("DEBUG T0\n");
  struct table * o = calloc(1, sizeof(struct table));
  //printf("DEBUG T1\n");
  strncpy(o->name, tablename, 63);
  //printf("DEBUG T2\n");
  o->colcount = colcount;
  o->rowcount = 0;
  o->columnnames = calloc(colcount, sizeof(char[64]));
  for(int i = 0; i < colcount; ++i){
    //printf("%s\n", columnnames[i]);
    strncpy(o->columnnames[i], columnnames[i], 63);
  }
  //printf("DEBUG T3\n");
  o->data = init_vector();
  //printf("DEBUG T4\n");
  return o;
}

void add_row(struct table * table, struct intvector * row){
  if(row->size != table->colcount){
    printf("ERROR: Tried to add row to table with size %d, expected colcount %d, program exiting.\n", row->size, table->colcount);
    exit(1);
  }
  add_vector(table->data, row);
  table->rowcount++;
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
    printf("Row %d: [", i);
    struct intvector * currow = (table->data->values)[i];
    for(int j = 0; j < table->colcount; j++){
      if(j != 0){
        printf(" ");
      }
      printf("%d",(currow->values[j]));
    }
    printf("]\n");
  }
}
