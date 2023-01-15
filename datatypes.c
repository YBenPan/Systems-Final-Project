#include "datatypes.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <limits.h>

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

// returns a char * pointer to a memory block of size get_datatype_size, returns NULL (graciously fails) if parsing fails
char * parse_string_to_data(char * strinput, struct datatype * dt){
  if(dt->type >= TOTAL_DATATYPE_COUNT){
    printf("ERROR: parse_string_to_data was passed a datatype with type %d, maximum type value cannot exceed %d, exiting!\n", dt->type, TOTAL_DATATYPE_COUNT);
    exit(1);
  }
  char * o = NULL;
  // note for numbers:
  // if overflow / underflow, ERROR, errno set to ERANGE
  // if string has non-string components, ERROR, errno set to EINVAL
  // hex numbers and octals NOT SUPPORTED, only decimals
  // all values are SIGNED
  if(dt->type == DATATYPE_INT || dt->type == DATATYPE_SHORTINT || dt->type == DATATYPE_TINYINT || dt->type == DATATYPE_LONG){
    if(strinput != '\0'){
      char * endptr;
      errno = 0;
      long long int rv = strtoll(strinput, &endptr, 10);
      if(errno != 0){
        if(**endptr == '\0'){
          long long int maxval;
          long long int minval;
          switch(dt->type){
            case DATATYPE_INT:
              maxval = INT_MAX;
              minval = INT_MIN;
              break;
            case DATATYPE_SHORTINT:
              maxval = SHRT_MAX;
              minval = SHRT_MIN;
              break;
            case DATATYPE_TINYINT:
              maxval = SCHAR_MAX;
              minval = SCHAR_MIN;
              break;
            case DATATYPE_LONG:
              maxval = LLONG_MAX;
              minval = LLONG_MIN;
              break;
          }
          if(rv > maxval || rv < minval){
            // out of bounds
            errno = ERANGE;
            o = NULL;
          } else {
            // actually good value
            // to avoid endianness issues, use typecasting here instead:
            switch(dt->type){
              case DATATYPE_INT:
                int * p = calloc(1, sizeof(int));
                *p = (int)(rv);
                o = (char *)(p);
                break;
              case DATATYPE_SHORTINT:
                short * p = calloc(1, sizeof(short));
                *p = (short)(rv);
                o = (char *)(p);
                break;
              case DATATYPE_TINYINT:
                signed char * p = calloc(1, sizeof(signed char));
                *p = (signed char)(rv);
                o = (char *)(p);
                break;
              case DATATYPE_LONG:
                long long * p = calloc(1, sizeof(long long));
                *p = rv;
                o = (char *)(p);
                break;
            }
          }
        } else {
          // invalid character somewhere
          errno = EINVAL;
          o = NULL;
        }
      } else {
        // error, pass it down
        o = NULL;
      }
    }
  } else if (dt->type == DATATYPE_FLOAT){
  } else if (dt->type == DATATYPE_DOUBLE){
  } else if (dt->type == DATATYPE_CHAR){
  } else if (dt->type == DATATYPE_TEXT){
  }
  return o;
}