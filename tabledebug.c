#include <stdio.h>

#include "vector.h"
#include "table.h"

int main(){
  char *colnames[3];
  colnames[0] = "col 0";
  colnames[1] = "col 1";
  colnames[2] = "col 2";
  struct table * table = init_table("testing", colnames, 3);
  struct intvector * row = init_intvector();
  for(int i = 0; i < 3; ++i){
    add_intvector(row, i + 7);
  }
  add_row(table, row);
  row = init_intvector();
  for(int i = 0; i < 3; ++i){
    add_intvector(row, i);
  }
  add_row(table, row);
  print_table(table);
}
