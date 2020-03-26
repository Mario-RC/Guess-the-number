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

  int id_cola, ret;

  struct msgbuf{
    long mtype;
    char mtext[15];
  } mensaje;

  /* Llamada a la función de creación de colas */
  id_cola = crearcola(Clave_cola);
  if(id_cola == -1){
    printf("No se ha podido crear la cola !\n");
    exit(EXIT_FAILURE);
  }

  /* Inicialización de los campos de la estructura */
  mensaje.mtype = 1; // Tipo de datos en la cola

  // Inicio del número de mensajes a mandar
  for (int i = 0;i<10;i++){

    sleep(1);
    sprintf(mensaje.mtext,"Mensaje %d",i); // Transforma el numero en cadena i->"i"

    /* Envío del mensaje (introducción en la cola) */
    ret = msgsnd(id_cola,&mensaje,sizeof(mensaje.mtext),0);
    if(ret == -1){
      perror("msgsnd");
      exit(EXIT_FAILURE);
    }

    printf("He enviado el mensaje: %s\n",mensaje.mtext);
  }
}
