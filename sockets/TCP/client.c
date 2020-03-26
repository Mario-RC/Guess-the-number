#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define SERVPORT 5001
#define BUFFER_SIZE 300

int main(int argc, char *argv[]){

  int sfd,p;
  int intentos = 10;
  struct sockaddr_in server_addr;
  struct hostent *host_name;
  char PredC[BUFFER_SIZE];
  char RespS[BUFFER_SIZE];

  // Crea el socket
  sfd = socket(AF_INET, SOCK_STREAM, 0);

  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(SERVPORT);
  //inet_aton("192.168.2.110",&server_addr.sin_addr);
  inet_aton("xxx.yyy.zzz.www",&server_addr.sin_addr);
  p = sizeof(server_addr);

  // Espera 500 ms a que se active el servidor primero
  usleep(500);
  // Pide conexión (a dirección IP/puerto)
  connect(sfd,(struct sockaddr *) &server_addr, sizeof(server_addr));

  printf("Tiene 10 intentos para acertar el número secreto entre 0 y 100\n\n");

  while (intentos > 0){

    // Escribir datos
    printf("Escriba un número: ");
    fgets(PredC,BUFFER_SIZE,stdin);
    write(sfd,(void *) PredC,BUFFER_SIZE);

    // Leer datos
    read(sfd,RespS,BUFFER_SIZE);
    printf("Respuesta del Servidor: %s\n\n",RespS);

    if(strcmp(RespS,"Acierto") == 0){ // Acierto
      printf("Lo has conseguido!!\n\nAdiós!!"); fflush(stdout);
      break;
    }

    intentos--;

    if(intentos == 0){
      printf("Te has quedado sin intentos, más suerte la próxima vez!!"); fflush(stdout);
      break;
    }
    else
      printf("Quedan %d intentos\n\n",intentos);

  } // end while

  sleep(4);

  // Cierre
  close(sfd);
  return (0);

} // end main
