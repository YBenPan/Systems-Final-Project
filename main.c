#include <stdio.h>
#include <stdlib.h>
#include "file_io.h"
#include "vector.h"
#include "table.h"
#include "parser.h"
#include "strcmds.h"

int main() {
  char *input = malloc(MAX_CMD_LENGTH);
  while (1) {
    // Prompt user for input 
    printf("Input global command:\n");
    fgets(input, MAX_CMD_LENGTH, stdin);
    chop_newline(input);

    int r = global_parser(input);
    if (r == -1) {
      printf("Database connection exiting!\n");
      break;
    }
  }
  return 0;
}
