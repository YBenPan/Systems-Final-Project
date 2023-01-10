#ifndef VECTOR_H
#define VECTOR_H

// vector of void * pointers
struct vector{
	int size;
	int capacity;
	void **values;
};

// vector of ints
struct intvector{
	int size;
	int capacity;
	int *values;
};

struct vector * init_vector();

void resize_vector(struct vector * v, int capacity);

void add_vector(struct vector *v, void *p);

// returns thing that was removed
void * remove_vector(struct vector *v);

// returns thing that was removed
void * delete_vector(struct vector *v, int index);

struct intvector * init_intvector();

void resize_intvector(struct intvector * v, int capacity);

void add_intvector(struct intvector *v, int x);

// returns thing that was removed
int remove_intvector(struct intvector *v);

// returns thing that was removed
int delete_intvector(struct intvector *v, int index);

#endif
