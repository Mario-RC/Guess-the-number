/* Programa de ejemplo del uso de colas de mensajes -- productor */

#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

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

/* Función principal */
int main(void){

  int id_cola,ret;
  int respC = 0;
  int predPMax = 256;
  int predPmin = 0;
  int predPuno = 0;

  srandom(getpid()); // Inicializa la semilla para los numeros
  int predP = (int)(256.0*random()/RAND_MAX); // Predicción inicial del padre, será un numero entre 0 y 256
  printf("Pred: %d\n",predP);

  struct msgbuf{
    long mtype;
    char mtext[15];
  } mensajeP,mensajeC;

  /* Llamada a la función de creación de colas */
  id_cola = crearcola(Clave_cola);
  if(id_cola == -1){
    printf("No se ha podido crear la cola !\n");
    exit(EXIT_FAILURE);}
 else
    printf("\nLa cola se ha creado correctamente\n");

  /* Inicialización de los campos de la estructura */
  mensajeP.mtype = 1; // Tipo de datos Productor en la cola
  mensajeC.mtype = 2; // Tipo de datos Consumidor en la cola

  // Inicio del número de mensajes a mandar
  for (int i = 0;i<10;i++){

    sleep(1);
    sprintf(mensajeP.mtext,"%d",predP); // Transforma el numero en cadena i->"i"
    /* Envío del mensaje (introducción en la cola) */
    ret = msgsnd(id_cola,&mensajeP,sizeof(mensajeP.mtext),0);
    if(ret == -1){
      perror("msgsnd");
      exit(EXIT_FAILURE);
    }

    /* Lectura de datos de la cola */
    ret = msgrcv(id_cola,&mensajeC,sizeof(mensajeC.mtext),2,0);
    if(ret == -1){
      perror("msgrcv");
      exit(EXIT_FAILURE);
    }
    respC = atoi(mensajeC.mtext); // Transforma la cadena en numero "i"->i

    if(respC == 3){ // Acierto
      printf("Productor: Lo he conseguido!!\n\n");
      break;
    }
    else if(respC == 1){ // Mayor respC = 1
      predPmin = predP;
      predP = predPMax - ((predPMax-predP)/2);
    }
    else if(respC == 2){ // Menor respC = 2
      predPMax = predP;
      predP = predP - ((predP-predPmin)/2);
      if(predP == 1){
        predPuno++;
        if(predPuno == 2)
          predP = 0;
      }
    }

    printf("Pred: %d\n",predP);

  } // end for
} // end main
