#include "file_io.h"
#include "vector.h"
#include "table.h"
#include "parser.h"

int main() {
  char input[MAX_CMD_LENGTH];
  usr_input(input);
  global_parser(input);
}
