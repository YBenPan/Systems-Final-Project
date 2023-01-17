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
#include "vector.h"
#include "parser.h"

struct intvector * cleanup_vec_process(char cleanup){
  static struct intvector *v = NULL;
  //printf("%p\n", v);
  if(cleanup){
    if(!v){
      exit(0);
    }
    // kill all subprocesses
    printf("SERVER: Cleaning up subprocesses before ending.\n");
    for(int i = 0; i < v->size; ++i){
      int targetpid = v->values[i];
      printf("SERVER: Killing subserver with PID %d\n", targetpid);
      kill(targetpid, SIGTERM);
    }
    exit(0);
  } else {
    // initing
    //printf("debug\n");
    v = init_intvector();
    //printf("%p\n", v);
  }
  return v;
}

static void sighandler(int signo){
  if(signo == SIGINT){
    // clean up stuff
    cleanup_vec_process(1);
  }
}

void server_process(int client_fd){
  while(1){
    printf("Input command to send to server:\n");
    char buff[MAX_EXCHANGE_LENGTH];
    read(client_fd, buff, MAX_EXCHANGE_LENGTH);
    global_parser(buff);
    /*
    char buff[MAX_EXCHANGE_LENGTH];
    read(client_fd, buff, MAX_EXCHANGE_LENGTH);
    char *output_buff = NULL;
    
    do_parse_client(buff, &output_buff);
    strncpy(buff, *output_buff, MAX_EXCHANGE_LENGTH - 1);
    buff[MAX_EXCHANGE_LENGTH-1] = '\0';
    write(client_fd, buff, strlen(buff) + 1);
    */
  }
}

void fork_processing_server(struct intvector *v, int client_fd){
  int fork_res = fork();
  if(fork_res == 0){
    printf("SERVER: Forked new child server with PID %d to deal with new client!\n", getpid());
    v->size = 0;
    dup2(client_fd, STDOUT_FILENO);
    dup2(client_fd, STDIN_FILENO);
    server_process(client_fd);
  } else {
    // og server, add pid to it
    add_intvector(v, fork_res);
  }
}

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
  printf("SERVER: Socket server setup! Listening on port %s\n", NETWORKING_PORT);
  free(hints);
  freeaddrinfo(res);
  return listening_fd;
}

int main(){
  signal(SIGINT, sighandler);
  struct intvector *v = cleanup_vec_process(0);
  int listening_fd = socket_server_setup();
  // more boilerplate code :(
  socklen_t sock_size;
  struct sockaddr_storage client_address;
  sock_size = sizeof(client_address);
  while(1){
    printf("SERVER: Waiting for client connection\n");
    int client_fd = accept(listening_fd, (struct sockaddr *)&client_address, &sock_size);
    fork_processing_server(v, client_fd);
  }
}
