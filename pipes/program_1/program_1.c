/* Programa de ejemplo del uso de pipes */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(void){

int fd[2];
int status;
int nread;
char buffer[100];

/* Creacion de la pipe */

if(pipe(fd) == -1){
  perror("pipe");
  exit(EXIT_FAILURE);
}
printf("Pipe OK!\n");

// Creacion de un proceso hijo

switch(fork())
{
  case -1:
    perror("fork");
    exit(EXIT_FAILURE);
  case 0: // Este es el hijo ¡
    // Cierre del descriptor de escritura de la pipe en el proceso hijo
    if(close(fd[1]) == -1)
      perror("close");
    // Lectura de la información contenida en la pipe.
    switch(nread = read(fd[0],buffer,sizeof(buffer))){
      case -1:
        perror("read");
        break;
      case 0:
        perror("EOF");
        break;
      default:
        printf("Hijo: He leido %d bytes, el mensaje de mi padre es '%s'.\n",nread,buffer);
        exit(EXIT_SUCCESS);
    }
  }

// Este es el padre

  /* Cierre del descriptor de lectura en el proceso padre */
  if(close(fd[0]) == -1)
    perror("close");

  /* El proceso padre escribe en la pipe */
  if(write(fd[1],"Hola hijo",9) == -1)
    perror("write");

  /* El proceso padre espera la finalizacion del proceso hijo */
  wait(&status);
  exit(EXIT_SUCCESS);
}
