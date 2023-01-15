#include "schema.h"
#include "vector.h"
#include "datatypes.h"

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

struct schema * init_schema(int colcount, struct vector * datatypes){
  struct schema * o = calloc(sizeof(struct schema));
  o->colcount = colcount;
  o->rowbytesize = NULL;
  if(datatypes->size != colcount){
    printf("ERROR: init_schema called with a datatypes vector of size %d, expected colcount size %d, exiting!\n", datatypes->size, colcount);
    exit(1);
  }
  o->datatypes = datatypes;
  recompute_rowbytesize(o);
  return o;
}

void recompute_rowbytesize(struct schema * schm){
  struct vector * dts = schm->datatypes;
  int sum = 0;
  int * newrowbytesize = calloc(schm->colcount + 1, sizeof(int));
  for(int i = 0; i < dts->colcount; ++i){
    newrowbytesize[i] = sum;
    struct datatype * dt = dts->values[i];
    sum += get_datatype_size(dt);
  }
  newrowbytesize[dts->colcount] = sum;
  free(schm->rowbytesize);
  schm->rowbytesize = newrowbytesize;
  return;
}

