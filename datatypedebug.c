#include "datatypes.h"

#include <stdio.h>
#include <errno.h>
#include <string.h>

void debug_tester(char * inputstring){
  struct datatype * dt = parse_string_to_datatype(inputstring);
  if(dt == NULL){
    printf("TEST: datatype: NULL\n");
  } else {
    printf("TEST: datatype: ");
    print_datatype(dt);
    printf("\n");
  }
}

int main(){
  debug_tester("INT");
  debug_tester("int");
  debug_tester("inta");
  debug_tester("int(12)");
  debug_tester("int(12");
  debug_tester("int(a");
  debug_tester("int(1a");
  debug_tester("smallint");
  debug_tester("TINYINT");
  debug_tester("CHAR");
  debug_tester("FLOAT");
  debug_tester("DOUBLE");
  debug_tester("TEXT"); // invalid since text must need args
  debug_tester("TEXT(64)");
  return 0;
}
