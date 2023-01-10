#include <string.h>
#include <stdio.h>
#include <stdlib.h>

int select_table(char *args) {

}

int create_table(char *args) {

}

int drop_table(char *args) {

}

void master_parser(char *input) {
  // Create a copy of input string
  char *input_str = malloc(sizeof(input));
  strcpy(input_str, input);

  // Get the command
  char *cmd = strsep(&input_str, " ");
  // printf("%s\n", cmd);
  // printf("%s\n", input_str);

  if (!strcmp(cmd, "SELECT")) {
    select_table(input_str);
  }
  else if (!strcmp(cmd, "CREATE")) {
    create_table(input_str);
  }
  else if (!strcmp(cmd, "DROP")) {
    drop_table(input_str);
  }
  else {
    printf("Invalid command '%s'!\n", cmd);
    exit(EXIT_FAILURE);
  }
}

int main() {
  master_parser("SELEC table1");
}
