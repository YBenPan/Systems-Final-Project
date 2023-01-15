#include <stdlib.h>
#include <stdio.h>
#include "vector.h"

struct vector * init_vector(){
  struct vector *v = malloc(sizeof(struct vector));
  v->size = 0;
  v->capacity = 64;
  v->values = malloc(sizeof(void *) * (v->capacity));
  return v;
}

void resize_vector(struct vector * v, int capacity){
  void **temp = realloc(v->values, sizeof(void *) * capacity);
  if(temp){
    v->values = temp;
    v->capacity = capacity;
  } else {
    free(v->values);
    printf("ERROR: REALLOC FOR VECTOR FAILED, EXITING");
    exit(1);
  }
}

void add_vector(struct vector *v, void *p){
  if(v->capacity == v->size){ // need resizing
    resize_vector(v, v->capacity * 2 + 1);
  }
  v->values[v->size] = p;
  v->size++;
}

void * remove_vector(struct vector *v){
  v->size--;
  return v->values[v->size];
}

void * delete_vector(struct vector *v, int index){
  void * copy = v->values[index];
  if(index >= 0 && index < v->size){
    for(int i = index; i < v->size - 1; i++){
      v->values[i] = v->values[i+1];
    }
    v->size--;
  }
  return copy;
}

struct intvector * init_intvector(){
  struct intvector *v = malloc(sizeof(struct intvector));
  v->size = 0;
  v->capacity = 64;
  v->values = malloc(sizeof(int) * (v->capacity));
  return v;
}

void resize_intvector(struct intvector * v, int capacity){
  int *temp = realloc(v->values, sizeof(int) * capacity);
  if(temp){
    v->values = temp;
    v->capacity = capacity;
  } else {
    free(v->values);
    printf("ERROR: REALLOC FOR INT VECTOR FAILED, EXITING");
    exit(1);
  }
}

void add_intvector(struct intvector *v, int x){
  if(v->capacity == v->size){ // need resizing
    resize_intvector(v, v->capacity * 2 + 1);
  }
  v->values[v->size] = x;
  v->size++;
}

int remove_intvector(struct intvector *v){
  v->size--;
  return v->values[v->size];
}

int delete_intvector(struct intvector *v, int index){
  int copy = v->values[index];
  if(index >= 0 && index < v->size){
    for(int i = index; i < v->size - 1; i++){
      v->values[i] = v->values[i+1];
    }
    v->size--;
  }
  return copy;
}
