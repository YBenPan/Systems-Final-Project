#include <fcntl.h>
#include <unistd.h>

#include "table.h"
#include "filo_io.h"

// REMEMBER TO UPDATE THIS WHEN YOU CHANGE FILE FORMAT
#define CURRENT_TABLE_FILE_VERSION 0

char write_table(struct table * table);

struct table * read_table(char * table_name);
