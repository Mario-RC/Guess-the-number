/* Programa de ejemplo del uso de colas de mensajes -- consumidor */

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
  int PredP = 0;
  int respC = 0;

  srandom(getpid()); // Inicializa la semilla para los numeros
  int numConsumidor = (int)(256.0*random()/RAND_MAX); // Número que elige el hijo, será un numero entre 0 y 256

  struct msgbuf{
    long mtype;
    char mtext[15];
  } mensajeP,mensajeC;

  /* Se conecta a la cola creada */
  id_cola = crearcola(Clave_cola);
  if(id_cola == -1){
    printf("No se ha podido crear la cola !\n");
    exit(EXIT_FAILURE);}
  else
    printf("\nSe ha conectado a la cola correctamente\n");

  printf("Num secreto consumidor: %d\n",numConsumidor);

  /* Inicialización de los campos de la estructura */
  mensajeP.mtype = 1; // Tipo de datos Productor en la cola
  mensajeC.mtype = 2; // Tipo de datos Consumidor en la cola

  // Inicio del número de mensajes a mandar
  for (int i = 0;i<10;i++){

    /* Lectura de datos de la cola */
    ret = msgrcv(id_cola,&mensajeP,sizeof(mensajeP.mtext),1,0);
    if(ret == -1){
      perror("msgrcv");
      exit(EXIT_FAILURE);
    }
    PredP = atoi(mensajeP.mtext); // Transforma la cadena en numero "i"->i

    if(numConsumidor == PredP){
      respC = 3;
      printf("Consumidor: Acierto!!\n");
    }
    else if(numConsumidor > PredP){
      respC = 1;
      printf("Mayor\n");
    }
    else if(numConsumidor < PredP){
      respC = 2;
      printf("Menor\n");
    }

    sprintf(mensajeC.mtext,"%d",respC); // Transforma el numero en cadena i->"i"
    /* Envío del mensajeC (introducción en la cola) */
    ret = msgsnd(id_cola,&mensajeC,sizeof(mensajeC.mtext),0);
    if(ret == -1){
      perror("msgsnd");
      exit(EXIT_FAILURE);
    }
    // Para salir del bucle for en caso de acertar
    if(respC == 3){
      break;
    }

  } // end for

  // Elimina la cola del sistema
  ret = msgctl(id_cola,IPC_RMID,(struct msqid_ds*)0);
  if(ret == -1)
    perror("msgctl");
  else
    printf("La cola se ha cerrado correctamente\n\n");
}
