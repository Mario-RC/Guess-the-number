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

  int id_cola, ret;

  struct msgbuf{
    long mtype;
    char mtext[15];
  } mensaje;

  /* Llamada a la función de creación de
     colas para conectarse a la cola creada */
  id_cola = crearcola(Clave_cola);
  if(id_cola == -1){
    printf("No se ha podido crear la cola !\n");
    exit(EXIT_FAILURE);
  }

  /* Inicialización de los campos de la estructura */
  mensaje.mtype = 1; // Tipo de datos en la cola

  // Inicio del número de mensajes a mandar
  for (int i = 0;i<10;i++){

    /* Lectura de datos de la cola */
    ret = msgrcv(id_cola,&mensaje,sizeof(mensaje.mtext),1,0);
    if(ret == -1){
      perror("msgrcv");
      exit(EXIT_FAILURE);
    }

    printf("He recibido el mensaje: %s\n",mensaje.mtext);
  }

  // Elimina la cola del sistema
  ret = msgctl(id_cola,IPC_RMID,(struct msqid_ds*)0);
  if(ret == -1)
    perror("msgctl");
  else
    printf("La cola se ha cerrado correctamente\n");
}
