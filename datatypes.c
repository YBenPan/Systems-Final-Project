#include "datatypes.h"

#include <stdio.h>
#include <unistd.h>

int get_datatype_size(struct datatype * dt){
  static int datatype_sizes[] = DATATYPE_SIZES;
  if(dt->type >= TOTAL_DATATYPE_COUNT){
    printf("ERROR: get_datatype_size was passed a datatype with type %d, maximum type value cannot exceed %d, exiting!\n", dt->type, TOTAL_DATATYPE_COUNT);
    exit(1);
  }
  int rv = datatype_sizes[dt->type];
  if(!rv){
    rv = dt->args;
  }
  return rv;
}
