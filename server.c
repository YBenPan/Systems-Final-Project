#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>

#include "error_handler.h"
#include "networking.h"

// just dump the boilerplate code in here
int socket_server_setup(){
  struct addrinfo * hints;
  hints = calloc(1, sizeof(struct addrinfo));
  struct addrinfo * res;
  hints->ai_family = AF_INET;
  hints->ai_socktype = SOCK_STREAM;
  hints->ai_flags = AI_PASSIVE;
  getaddrinfo(NULL, NETWORKING_PORT, hints, &res);
  int listening_fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
  ERROR_HANDLER(listening_fd, " in socket() when setting up sockets");
  int yes = 1;
  ERROR_HANDLER(setsockopt(listening_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)), " in setsockopt() when setting up sockets");
  ERROR_HANDLER(bind(listening_fd, res->ai_addr, res->ai_addrlen), " in bind() when setting up sockets");
  listen(listening_fd, SERVER_MAX_CLIENT_COUNT);
  printf("Socket server setup! Listening on port %s\n", NETWORKING_PORT);
  return listening_fd;
}

int main(){
  int listening_fd = socket_server_setup();
}
