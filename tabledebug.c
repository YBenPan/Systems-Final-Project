#include <stdio.h>

#include "file_io.h"
#include "vector.h"
#include "table.h"
#include "datatypes.h"
#include "schema.h"

int main(){
  char *colnames[4];
  colnames[0] = "col 0";
  colnames[1] = "col 1";
  colnames[2] = "delimiter, \"testing\" column";
  colnames[3] = "col 3";
  printf("DEBUG 0\n");
  struct schema * schm = init_schema_from_text(4, "int double TINYINT TEXT(8)");
  printf("DEBUG 1\n");
  print_schema(schm);
  printf("DEBUG 2\n");
  struct table * table = init_table("testing", colnames, 4, schm);
  printf("DEBUG 3\n");
  char *additionalvalues[4];
  additionalvalues[0] = "718";
  additionalvalues[1] = "129312318301";
  additionalvalues[2] = "317";
  additionalvalues[3] = "hello";
  char retval = add_row_from_text(table, schm, additionalvalues);
  printf("RETVAL: %d\n", retval); // should fail
  additionalvalues[2] = "100";
  retval = add_row_from_text(table, schm, additionalvalues);
  printf("RETVAL: %d\n", retval); // should succeed
  print_table(table);
  additionalvalues[3] = "hello world";
  retval = add_row_from_text(table, schm, additionalvalues);
  printf("RETVAL: %d\n", retval); // should fail
  print_table(table);
  additionalvalues[1] = "-928131230.123";
  additionalvalues[2] = "-123";
  additionalvalues[3] = "abcdefg";
  retval = add_row_from_text(table, schm, additionalvalues);
  printf("RETVAL: %d\n", retval); // should succeed
  additionalvalues[1] = "-928131230.123a";
  retval = add_row_from_text(table, schm, additionalvalues);
  printf("RETVAL: %d\n", retval); // should fail
  print_table(table);
  printf("Trying to write table to file testing.tbl\n");
  write_table(table);
  struct table * table2 = read_table("testing");
  printf("table 2 COLCOUNT: %d\n", table2->colcount);
  printf("table 2 ROWCOUNT: %d\n", table2->rowcount);
  print_table(table2);
  printf("Writing table to tabletest.csv\n");
  write_table_to_csv(table2, "tabletest.csv");
/*
  works!
  struct table * table2 = read_table("testingv0");
  print_table(table2);
*/
/*
  //printf("DEBUG 1\n");
  struct intvector * row = init_intvector();
  //printf("DEBUG 2\n");
  for(int i = 0; i < 3; ++i){
    add_intvector(row, i + 7);
  }
  //printf("DEBUG 3\n");
  add_row(table, row);
  //printf("DEBUG 4\n");
  row = init_intvector();
  for(int i = 0; i < 3; ++i){
    add_intvector(row, i);
  }
  //printf("DEBUG 5\n");
  add_row(table, row);
  //printf("DEBUG 6\n");
  print_table(table);
  //printf("DEBUG 7\n");
  printf("Trying to write table to file testing.tbl\n");
  write_table(table);
  struct table * table2 = read_table("testing");
  //printf("COLCOUNT: %d\n", table2->colcount);
  //printf("ROWCOUNT: %d\n", table2->rowcount);
  print_table(table2);
  row = init_intvector();
  for(int i = 0; i < 3; ++i){
    add_intvector(row, 10-i);
  }
  printf("Adding 2 more rows:\n");
  add_row(table2, row);
  row = init_intvector();
  for(int i = 0; i < 3; ++i){
    add_intvector(row, 12-i);
  }
  add_row(table2, row);
  print_table(table2);
  printf("Writing table to tabletest.csv\n");
  write_table_to_csv(table2, "tabletest.csv");
*/
  return 0;
}
