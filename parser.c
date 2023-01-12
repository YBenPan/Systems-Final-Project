#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int select_table(char *args) {
  // Process args and put together file path
  char *table_name = strsep(&args, " ");
  char *file_dir = "./";
  char *file = malloc(sizeof(table_name) + 1);
  strcpy(file, file_dir);
  file = strcat(file, table_name);
  printf("%s\n", file);

  // Open table
  int fd = open(file, O_RDONLY);
  if (fd == -1) {
    printf("Table '%s' not found!\n", table_name);
    exit(errno);
  }

  // TODO: Select table. If statements to direct function to more operations

  close(fd);
}

int create_table(char *args) {

}

int drop_table(char *args) {

}

void usr_input(char *input) {
  printf("Input table command:\n");
  fgets(input, 255, stdin);
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
  char input[255];
  usr_input(input);
  master_parser(input);
}
