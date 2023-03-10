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
#include "strcmds.h"

void send_message_to_server(int estb_socket, char * msg){
  static int socket_fd = -1;
  if(estb_socket != -1){
    socket_fd = estb_socket;
  }
  if((socket_fd != -1) && msg){
    // for now just write to buff, include term newline, read everything
    write(socket_fd, msg, strlen(msg) + 1);
  }
}

static void sighandler(int signo){
  if(signo == SIGINT){
    // clean up client
    send_message_to_server(-1, "TERMINATE");
    exit(0);
  }
}

// also dump boilerplate code in here
int client_socket_setup(char * targetaddress){
  struct addrinfo * hints;
  hints = calloc(1, sizeof(struct addrinfo));
  struct addrinfo * res;
  hints->ai_family = AF_INET;
  hints->ai_socktype = SOCK_STREAM;
  int gai_errcode = getaddrinfo(targetaddress, NETWORKING_PORT, hints, &res);
  //printf("test\n");
  if(gai_errcode){
    printf("ERROR occurred in getaddrinfo: %s\n", gai_strerror(gai_errcode));
    exit(0);
  }
  int socket_fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
  //printf("test2\n");
  ERROR_HANDLER(socket_fd, " when trying to establish socket_fd");
  ERROR_HANDLER(connect(socket_fd, res->ai_addr, res->ai_addrlen), " when trying to connect to socket");
  return socket_fd;
}

int main(int argc, char * argv[]){
  signal(SIGINT, sighandler);
  char * targetaddr = NULL;
  if(argc < 2){
    printf("CLIENT: Target address not provided, using localhost as default.\n");
  } else {
    targetaddr = argv[1];
  }
  //printf("TARGETADDR: %s\n", targetaddr);
  int socket_fd = client_socket_setup(targetaddr);
  printf("CLIENT: Established connection with server!\n");
  send_message_to_server(socket_fd, NULL);
  while(1){
    char buff[MAX_EXCHANGE_LENGTH];
    //printf("CLIENT: waiting for server\n");
    int br = read(socket_fd, buff, MAX_EXCHANGE_LENGTH);
    ERROR_HANDLER(br, " when reading from server");
    int *p = (int *)buff;
    //printf("CLIENT debug: %x\n", *p);
    if(*p == CLIENT_REQUEST_INPUT_NONCE){
      //printf("CLIENT: Server requested input from client.\n");
      fgets(buff, MAX_EXCHANGE_LENGTH, stdin);
      chop_newline(buff);
      send_message_to_server(socket_fd, buff);
    } else {
      if(br == MAX_EXCHANGE_LENGTH){
        br--;
      }
      buff[br] = '\0';
      printf("%s", buff);
    }
  }
}
