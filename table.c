#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "table.h"
#include "vector.h"

struct table * init_table(char * tablename, char **columnnames, int colcount){
	struct table * o = calloc(1, sizeof(struct table));
  strncpy(o->name, tablename, 63);
  o->colcount = colcount;
  o->rowcount = 0;
  for(int i = 0; i < colcount; ++i){
    strncpy(o->columnnames[i], columnnames[i], 63);
  }
  o->data = init_vector();
  return o;
}

void add_row(struct table * table, struct intvector * row){
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
