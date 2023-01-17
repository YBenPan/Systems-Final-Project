#ifndef ERROR_HANDLER_H
#define ERROR_HANDLER_H

#include <errno.h>
#include <string.h>
#include  <stdio.h>
#include <stdlib.h>

#define ERROR_HANDLER(val, msg){\
	if((val) == -1){\
		printf("ERROR OCCURED%s: %s, program exiting!\n", msg, strerror(errno));\
		exit(1);\
	}\
}

#endif
