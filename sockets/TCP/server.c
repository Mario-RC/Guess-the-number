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
  int option = 1; // Poder reutilizar el socket inmediatamente una vez cerrado
  struct sockaddr_in server_addr;
  struct sockaddr_in client_addr;
  char PredC[BUFFER_SIZE];
  char RespS[BUFFER_SIZE];
  int predc;
  p = sizeof(client_addr);

  srandom(getpid()); // Inicializa la semilla para los numeros casuales
  int numConsumidor = (int)(100.0*random()/RAND_MAX); // Número que elige el hijo, será un numero entre 0 y 256
  printf("Num secreto consumidor: %d\n",numConsumidor);

  // Crea el socket para el cliente
  sfd = socket(AF_INET, SOCK_STREAM, 0);
  setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));

  /*Se nombra el socket de acuerdo con el servidor*/
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  server_addr.sin_port = htons(SERVPORT);

  // Asigna dirección IP, asociar un puerto al socket (bind())
  if(bind(sfd,(struct sockaddr *) &server_addr,sizeof(server_addr)) == -1){
    perror("Fallo con bind");
    exit(1);
  } // end if

  // Empieza a escuchar (espera conexión cliente)
  listen(sfd, 5);

  // Acepta la conexión
  sfd = accept(sfd,(struct sockaddr*) NULL,NULL);

  while (intentos > 0){

    // Leer datos
    printf("Servidor esperando...\n\n");
    read(sfd,PredC,BUFFER_SIZE);
    predc = atoi(PredC); // Transforma la cadena en numero "i"->i
    printf("Número del cliente: %d\n",predc);

    if(numConsumidor == predc){
      strcpy(RespS,"Acierto");
      printf("Acierto!!\n\nHasta la próxima!!"); fflush(stdout);
    }
    else if(numConsumidor > predc){
      strcpy(RespS,"Mayor");
      printf("Mayor\n");
    }
    else if(numConsumidor < predc){
      strcpy(RespS,"Menor");
      printf("Menor\n");
    }

    // Escribir datos
    write(sfd,(void *) RespS,BUFFER_SIZE);

    // Para salir del bucle while en caso de acertar
    if(strcmp(RespS,"Acierto") == 0){
      break;
    }

    intentos--;

    if(intentos == 0){
      printf("Te has quedado sin intentos, más suerte la próxima vez!!"); fflush(stdout);
      break;
    }
    else
      printf("Quedan %d intentos\n\n",intentos);

  }

  sleep(4);

  // Cierre
  close(sfd);
  return 0;

} // end main
