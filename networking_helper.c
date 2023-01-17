#include <unistd.h>
#include <stdio.h>
#include "networking.h"

void send_client_request_input_nonce(){
  fflush(stdout);
  // make sure nonce is sent separately and received separately
  // 1 second delay is unnoticable with user input anyways
  sleep(1);
  int nonce = CLIENT_REQUEST_INPUT_NONCE;
  //write(STDOUT_FILENO, &nonce, sizeof(int));
  fwrite(&nonce, sizeof(int), 1, stdout);
  fflush(stdout);
}
