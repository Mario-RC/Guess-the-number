/* Programa de ejemplo del uso de colas de mensajes -- consumidor */

#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/* Definición de la clave de la cola */
#define Clave_cola 1

/* Función auxiliar que crea la cola, retornando el identificador */
int crearcola(int key){
  int msgqid;
  msgqid = msgget(key, IPC_CREAT|0666);
  switch(msgqid){
    case -1:
      perror("msgget");
      return(-1);
    default:
      return msgqid;
  }
} /* Fin función auxiliar */

int main(void){

  int id_cola,ret,status;
  int Pred = 0;
  int PredP = 0;
  int RespH = 0;
  int respH = 0;
  int predPMax = 100;
  int predPmin = 0;
  int predPuno = 0;

  struct msgbuf{
    long mtype;
    char mtext[15];
  } mensajeP,mensajeH;

  /* Inicialización de los campos de la estructura */
  mensajeP.mtype = 1; // Tipo de datos Productor en la cola
  mensajeH.mtype = 2; // Tipo de datos Consumidor en la cola

  // Creacion de un proceso hijo
  switch(fork()){

    case -1:
      perror("fork");
      exit(EXIT_FAILURE);

    case 0: // Este es el hijo
      srandom(getpid()); // Inicializa la semilla para los numeros
      int numConsumidor = (int)(100.0*random()/RAND_MAX); // Número que elige el hijo, será un numero entre 0 y 100

      /* Se conecta a la cola creada */
      id_cola = crearcola(Clave_cola);
      if(id_cola == -1){
        printf("El hijo no se ha podido conectar a la cola !\n");
        exit(EXIT_FAILURE);}
      else
        printf("El hijo se ha conectado a la cola correctamente\n\n");
      //printf("Num secreto Hijo: %d\n",numConsumidor);

      /* Inicio del bucle de 100 intentos hijo */
      for (int i = 0;i<100;i++){

        //sleep(1);
        /* El proceso hijo lee al padre */
        switch(ret = msgrcv(id_cola,&mensajeP,sizeof(mensajeP.mtext),1,0)){

          case -1:
            perror("read");
            break;

          case 0:
            perror("EOF");
            break;

          default:
            if(ret == -1){
              perror("msgrcv");
              exit(EXIT_FAILURE);
            }
            PredP = atoi(mensajeP.mtext); // Transforma la cadena en numero "i"->i

            if(numConsumidor == PredP){
              respH = 3;
              printf("Hijo: Acierto!!\n");
            }
            else if(numConsumidor > PredP){
              respH = 1;
              printf("Mayor\n");
            }
            else if(numConsumidor < PredP){
              respH = 2;
              printf("Menor\n");
            }

            sprintf(mensajeH.mtext,"%d",respH); // Transforma el numero en cadena i->"i"
            /* Envío del mensajeH (introducción en la cola) */
            ret = msgsnd(id_cola,&mensajeH,sizeof(mensajeH.mtext),0);
            if(ret == -1){
              perror("msgsnd");
              exit(EXIT_FAILURE);
            }

         } // end switch

         // Para salir del bucle for en caso de acertar
         if(respH == 3){
           break;
         }

      } // end for

      usleep(500);

      // Elimina la cola del sistema
      ret = msgctl(id_cola,IPC_RMID,(struct msqid_ds*)0);
      if(ret == -1)
        perror("msgctl");
      else
        printf("El hijo ha cerrado correctamente la cola\n");

      printf("Termina el Hijo\n");
      exit(EXIT_SUCCESS); // Exit del Hijo


    default: // Este es el padre

      srandom(getpid()); // inicializa la semilla para los numeros
      int predP = (int)(100.0*random()/RAND_MAX); // Predicción inicial del padre, será un numero entre 0 y 100

      /* Llamada a la función de creación de colas */
      id_cola = crearcola(Clave_cola);
      if(id_cola == -1){
        printf("El padre no ha podido crear la cola !\n");
        exit(EXIT_FAILURE);}
      else
        printf("\nEl padre ha creado la cola correctamente\n");

      usleep(500);

      // Inicio del número de mensajes a mandar
      for (int i = 0;i<100;i++){

        printf("Pred: %d\n",predP);

        sprintf(mensajeP.mtext,"%d",predP); // Transforma el numero en cadena i->"i"
        /* Envío del mensaje (introducción en la cola) */
        ret = msgsnd(id_cola,&mensajeP,sizeof(mensajeP.mtext),0);
        if(ret == -1){
          perror("msgsnd");
          exit(EXIT_FAILURE);
        }

        //sleep(1);
        /* Lectura de datos de la cola */
        ret = msgrcv(id_cola,&mensajeH,sizeof(mensajeH.mtext),2,0);
        if(ret == -1){
          perror("msgrcv");
          exit(EXIT_FAILURE);
        }
        RespH = atoi(mensajeH.mtext); // Transforma la cadena en numero "i"->i

        if(RespH == 3){ // Acierto
          printf("Padre: Lo he conseguido!!\n\n");
          break;
        }
        else if(RespH == 1){ // Mayor RespH = 1
          predPmin = predP;	
          predP = predPMax - ((predPMax-predP)/2);
        }
        else if(RespH == 2){ // Menor RespH = 2
          predPMax = predP;
          predP = predP - ((predP-predPmin)/2);
          if(predP == 1){
            predPuno++;
            if(predPuno == 2)
              predP = 0;
          }
        }
      } // end for	

      /* El proceso padre espera la finalizacion del proceso hijo */
      wait(&status);

      printf("Termina el Padre\n");
      exit(EXIT_SUCCESS); // Exit del Padre

  } // end switch
} // end main
