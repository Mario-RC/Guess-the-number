/* Programa de ejemplo del uso de pipes */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>

int main(void){

  int fdPH[2],fdHP[2];
  int status,nreadPH,nreadHP;
  char bufferP[100];
  char bufferH[100];
  char msgP[100];
  char msgH[100];
  int predPadremin = 0;
  int predPadreMax = 256;
  int predPadreuno = 0;
  int respHijo;
  int predPadre,PredPadre,numHijo,RespHijo;

  /* Creacion de la pipe del Padre al Hijo */
  if(pipe(fdPH) == -1){
    perror("pipePH");
    exit(EXIT_FAILURE);
  }
  printf("\nPipe Padre OK!\n");

  /* Creacion de la pipe del Hijo al Padre */
  if(pipe(fdHP) == -1){
    perror("pipeHP");
    exit(EXIT_FAILURE);
  }
  printf("Pipe Hijo OK!\n\n");

  // Creacion de un proceso hijo
  switch(fork()){

    case -1:
      perror("fork");
      exit(EXIT_FAILURE);

    case 0: // Este es el hijo
      srandom(getpid()); // inicializa la semilla para los numeros
      numHijo = (int)(256.0*random()/RAND_MAX); // Número que elige el hijo, será un numero entre 0 y 256

      // Cierre del descriptor de escritura de la pipe en el proceso hijo
      if(close(fdPH[1]) == -1)
        perror("close");

      // Cierre del descriptor de lectura de la pipe en el proceso hijo
      if(close(fdHP[0]) == -1)
        perror("close");

      /* Inicio del bucle de 100 intentos hijo */
      for(int i = 0;i<100;i++){

        /* El proceso hijo lee al padre */
        switch(nreadPH = read(fdPH[0],bufferH,sizeof(bufferH))){

          case -1:
            perror("read");
            break;

          case 0:
            perror("EOF");
            break;

          default:
            bufferH[nreadPH]='\0';
            PredPadre = atoi(bufferH); // Transforma la cadena en numero "128"->128

            if(numHijo == PredPadre){
              respHijo = 3;
              printf("Hijo: Acierto!!\n");
            }
            else if(numHijo > PredPadre){
              respHijo = 1;
              printf("Mayor\n");
            }
            else if(numHijo < PredPadre){
              respHijo = 2;
              printf("Menor\n");
            }
  
            /* El proceso hijo escribe al padre */
            sprintf(msgH,"%d",respHijo); // Transforma el numero en cadena 128->"128"
            if(write(fdHP[1],msgH,strlen(msgH)) == -1)
              perror("write");
        }

        // Para salir del bucle for en caso de acertar
        if(respHijo == 3){
          break;
        }
      }

      // Cierre del descriptor de lectura de la pipe en el proceso hijo
      if(close(fdPH[0]) == -1)
        perror("close");

      // Cierre del descriptor de escritura de la pipe en el proceso hijo
      if(close(fdHP[1]) == -1)
        perror("close");

      exit(EXIT_SUCCESS); // Exit del Hijo

    default: // Este es el padre
      srandom(getpid()); // inicializa la semilla para los numeros
      predPadre = (int)(256.0*random()/RAND_MAX); // Predicción inicial del padre, será un numero entre 0 y 256
      printf("Pred: %d\n",predPadre);

      /* Cierre del descriptor de lectura en el proceso padre */
      if(close(fdPH[0]) == -1)
        perror("close");

      /* Cierre del descriptor de escritura en el proceso padre */
      if(close(fdHP[1]) == -1)
        perror("close");

      /* Inicio del bucle de 100 intentos padre */
      for(int i = 0;i<100;i++){

        /* El proceso padre escribe al hijo */
        sprintf(msgP,"%d",predPadre); // Transforma el numero en cadena 128->"128"
        if(write(fdPH[1],msgP,strlen(msgP)) == -1)
          perror("write");

        /* El proceso padre lee al hijo */
        if((nreadHP = read(fdHP[0],bufferP,sizeof(bufferP))) == -1)
          perror("read");
        bufferP[nreadHP]='\0';
        RespHijo = atoi(bufferP); // Transforma la cadena en numero "128"->128

        if(RespHijo == 3){ // Acierto
          printf("Padre: Lo he conseguido!!\n\n");
          break;
        }
        else if(RespHijo == 1){ // Mayor
          predPadremin = predPadre;
          predPadre = predPadreMax - ((predPadreMax-predPadre)/2);
        }
        else if(RespHijo == 2){ // Menor
          predPadreMax = predPadre;
          predPadre = predPadre - ((predPadre-predPadremin)/2);
          if(predPadre == 1){
            predPadreuno++;
            if(predPadreuno == 2)
              predPadre = 0;
          }
        }
        printf("Pred: %d\n",predPadre);
      }

      /* Cierre del descriptor de lectura en el proceso padre */
      if(close(fdPH[1]) == -1)
        perror("close");

      /* Cierre del descriptor de escritura en el proceso padre */
      if(close(fdHP[0]) == -1)
        perror("close");

      /* El proceso padre espera la finalizacion del proceso hijo */
      wait(&status);

      exit(EXIT_SUCCESS); // Exit del Padre
  }
}
