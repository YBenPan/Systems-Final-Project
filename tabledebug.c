#include <stdio.h>

#include "vector.h"
#include "table.h"

int main(){
  char *colnames[3];
  colnames[0] = "col 0";
  colnames[1] = "col 1";
  colnames[2] = "col 2";
  printf("DEBUG 0\n");
  struct table * table = init_table("testing", colnames, 3);
  printf("DEBUG 1\n");
  struct intvector * row = init_intvector();
  printf("DEBUG 2\n");
  for(int i = 0; i < 3; ++i){
    add_intvector(row, i + 7);
  }
  printf("DEBUG 3\n");
  add_row(table, row);
  printf("DEBUG 4\n");
  row = init_intvector();
  for(int i = 0; i < 3; ++i){
    add_intvector(row, i);
  }
  printf("DEBUG 5\n");
  add_row(table, row);
  printf("DEBUG 6\n");
  print_table(table);
  printf("DEBUG 7\n");
}
