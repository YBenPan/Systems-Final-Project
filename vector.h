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

#endif
