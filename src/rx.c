#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

int main(int argc, char** argv){
  int sd;
  int acc_sd;
  struct sockaddr_in addr;

  socklen_t sin_size = sizeof(struct sockaddr_in);
  struct sockaddr_in from_addr;

  char buf[2048];

  // init rcvd buff
  memset(buf, 0, sizeof(buf));

  // create IPv4 TCP socket
  if((sd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    perror("socket");
    return -1;
  }

  // set IP and port for wait
  addr.sin_family = AF_INET;
  addr.sin_port = htons(12000);
  addr.sin_addr.s_addr = INADDR_ANY;

  // bind
  if(bind(sd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
  perror("bind");
  return -1;
  }

  // to be wait for connection
  // wait que upto 10
  if(listen(sd, 10) < 0) {
    perror("listen");
    return -1;
  }

  // stop until recieve connection request from client
  // use acc_sd for TX and RX 
  if((acc_sd = accept(sd, (struct sockaddr *)&from_addr, &sin_size)) < 0) {
    perror("accept");
    return -1;
  }
 
  // Recieve Packet
  if(recv(acc_sd, buf, sizeof(buf), 0) < 0) {
    perror("recv");
    return -1;
  }

  // close tcp socket for RX data
  close(acc_sd);

  // close request socket
  close(sd);

  // output recieved socket
  printf("%s\n", buf);

  return 0;
}
