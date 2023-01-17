#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>

#include "error_handler.h"
#include "networking.h"

// also dump boilerplate code in here
void client_socket_setup(){
  struct addrinfo * hints;
  hints = calloc(1, sizeof(struct addrinfo));
  struct addrinfo * res;
  hints->ai_family = AF_INET;
  hints->ai_socktype = SOCK_STREAM;
  getaddrinfo(NULL, NETWORKING_PORT, hints, &res);
  int listening_fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
}

int main(){
}
