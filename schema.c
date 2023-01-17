#include "schema.h"
#include "vector.h"
#include "datatypes.h"

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

struct schema * init_schema(int colcount, struct vector * datatypes){
  struct schema * o = calloc(1, sizeof(struct schema));
  o->colcount = colcount;
  o->rowbytesize = NULL;
  if(datatypes->size != colcount){
    printf("ERROR: init_schema called with a datatypes vector of size %d, expected colcount size %d, exiting!\n", datatypes->size, colcount);
    return 0;
  }
  o->datatypes = datatypes;
  recompute_rowbytesize(o);
  return o;
}

// colcount -> expected column count, text -> space separated string of datatypes, if error, returns NULL
struct schema * init_schema_from_text(int colcount, char * text){
  char *tc = calloc(strlen(text) + 1, sizeof(char));
  char *tcog = tc;
  strcpy(tc, text);
  struct vector * datatypes = init_vector();
  char * tok = tc;
  while((tok = strsep(&tc, " "))){
    //printf("DEBUG TOK = %s\n", tok);
    struct datatype * dt = parse_string_to_datatype(tok);
    if(!dt){
      printf("ERROR: In init_schema_from_text, parse_string_to_datatype failed to parse datatype string %s\n", tok);
      free(datatypes->values);
      free(datatypes);
      free(tcog);
      return NULL;
    }
    add_vector(datatypes, dt);
  }
  if(datatypes->size != colcount){
    printf("ERROR: init_schema_from_text called with a text string with %d datatypes, expected colcount size %d\n", datatypes->size, colcount);
    for(int i = 0; i < datatypes->size; ++i){
      free(datatypes->values[i]);
    }
    free(tcog);
    free(datatypes->values);
    free(datatypes);
    return NULL;
  }
  free(tcog);
  return init_schema(colcount, datatypes);
}

void recompute_rowbytesize(struct schema * schm){
  struct vector * dts = schm->datatypes;
  int sum = 0;
  int * newrowbytesize = calloc(schm->colcount + 1, sizeof(int));
  for(int i = 0; i < schm->colcount; ++i){
    newrowbytesize[i] = sum;
    struct datatype * dt = dts->values[i];
    sum += get_datatype_size(dt);
  }
  newrowbytesize[schm->colcount] = sum;
  free(schm->rowbytesize);
  schm->rowbytesize = newrowbytesize;
  return;
}

void print_schema(struct schema * schm){
  printf("SCHEMA:\n");
  printf("colcount: %d\n", schm->colcount);
  printf("rowbytesize: [");
  for(int i = 0; i <= schm->colcount; ++i){
    if(i != 0){
      printf(", ");
    }
    printf("%d", schm->rowbytesize[i]);
  }
  printf("]\ndatatypes: [");
  for(int i = 0; i < schm->colcount; ++i){
    if(i != 0){
      printf(", ");
    }
    print_datatype(schm->datatypes->values[i]);
  }
  printf("]\n");
}
