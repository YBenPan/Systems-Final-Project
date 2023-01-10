#include <string.h>
#include <stdio.h>
#include <stdlib.h>

void master_parser(char *input) {
  // Create a copy of input string
  char *input_str = malloc(sizeof(input));
  strcpy(input_str, input);

  // Get the command
  char *cmd = strsep(&input_str, " ");
  printf("%s\n", cmd);
  printf("%s\n", input_str);

}

int main() {
  master_parser("TABLE table1");
}
