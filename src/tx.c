#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define LNG 23


int main(int argc, char** argv)
{
  int sd;
  struct sockaddr_in addr;

  // creat IPv4 TCP socket
  if((sd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    perror("socket");
    return -1;
  }

  // set port and address    addr.sin_family = AF_INET;
  addr.sin_port = htons(8001);
  addr.sin_addr.s_addr = inet_addr("127.0.0.1");

  // connect to server
  connect(sd, (struct sockaddr *)&addr, sizeof(struct sockaddr_in));

  // prepare binary data
  //    int d[32] = {0xC0, 0x00, 0x54, 0x45, 0x53, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0xDB};
  char d[LNG] = {0xC0,0x00,'J','I','1','K','V','E',' ','>','J','Q','1','Z','O','I',' ',':','R','S','T','5',0xC0};


  // tx TCP packet/*
  int i;
  for(i=0; i<LNG; i++){
    if(send(sd, d, 2, 0) < 0) {perror("send");return -1;}
  }
    send(sd, d, LNG, 0);


    // exit kiss connection
    //    int exitkisscommand[3] = {0xC0, 0xFF, 0xC0};
    //    send(sd, &exitkisscommand, 3, 0);

    close(sd);

    return 0;
}


