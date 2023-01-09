#ifndef TABLE_H
#define TABLE_H

#include "vector.h"

struct table{
	char name[64];
	char (*columnnames)[64];
	struct vector * values;
};

#endif
