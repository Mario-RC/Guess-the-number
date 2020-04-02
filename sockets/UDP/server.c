
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define SERVPORT 5000
#define BUFFER_SIZE 300

int main(int argc, char *argv[]){

  int ID_serv, p;
  struct sockaddr_in server_addr;
  struct sockaddr_in client_addr;
  char chain2[BUFFER_SIZE];
  p = sizeof(client_addr);

  /*Se crea un socket para el cliente*/
  ID_serv = socket(AF_INET, SOCK_DGRAM, 0);

  /*Se nombra el socket de acuerdo con el servidor*/
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  server_addr.sin_port = htons(SERVPORT);

  //Asociar un puerto al socket (bind())
  if(bind(ID_serv, (struct sockaddr *) &server_addr,sizeof(server_addr)) == -1){
    perror("Fallo con bind");
    exit(1);
  } // end if

  while (1){
    printf("Servidor esperando...\n");
    recvfrom(ID_serv, (char *) chain2, BUFFER_SIZE,0,(struct sockaddr*)&client_addr,&p);
    printf("%s", chain2);
    sendto(ID_serv, (char *) chain2, strlen(chain2)+1,0,(struct sockaddr*)&client_addr,sizeof(client_addr));
  } // end while

  close(ID_serv);

  return 0;

} // end main
