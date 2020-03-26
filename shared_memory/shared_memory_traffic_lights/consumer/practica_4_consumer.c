/* Programa de ejemplo del uso de colas de mensajes -- consumidor */

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <sys/sem.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

/* Fichero que contiene la información común (clave y estrucura de datos) */
#include "../common.h"

/* Función principal */
int main(void){

  int shmid,res;
  datos *zona_comun;
  int semid;
  struct sembuf operacion;
  int i = 100;

  srandom(getpid()); // Inicializa la semilla para los numeros casuales
  int numConsumidor = (int)(100.0*random()/RAND_MAX); // Número que elige el hijo, será un numero entre 0 y 256
  printf("Num secreto consumidor: %d\n",numConsumidor);

  /* Conexión a la zona de memoria compartida */
  shmid = shmget(CLAVE_SHM, sizeof(datos), IPC_CREAT|0666);
  if(shmid== -1){
    perror("shmget");
    exit(EXIT_FAILURE);
  }

  zona_comun = (datos*) shmat(shmid,0,0);

  // Creación de los semáforos
  semid = semget(CLAVE,2,IPC_CREAT|0600);
  if(semid == -1){
    perror("semget");
    exit(EXIT_FAILURE);
  }

  // El proceso consumidor NO inicializa los semáforos !
  printf("Valor semáforo 1: %d\n", semctl(semid,SEM_1,GETVAL,0));
  printf("Valor semáforo 2: %d\n\n", semctl(semid,SEM_2,GETVAL,0));
  operacion.sem_flg=0; /* No activamos ninguna opcion */

  // Inicio del número de mensajes a mandar
  while(i > 0){

    i--;

    // Consumidor espera SEM_2 (WAIT)
    //printf("B: espero SEM_2\n");
    operacion.sem_num = SEM_2; // que semaforo (puede haber más de 1!)
    operacion.sem_op = -1; // que operación: +1=signal, -1=wait
    semop(semid,&operacion,1); // envía el comando
    //printf("Proceso B dentro ! (%d)\n",i);

    if(numConsumidor == zona_comun->PredP){
      strcpy(zona_comun->RespC,"Acierto");
      printf("Consumidor: Acierto!!\n\n");
    }
    else if(numConsumidor > zona_comun->PredP){
      strcpy(zona_comun->RespC,"Mayor");
      printf("Mayor\n");
    }
    else if(numConsumidor < zona_comun->PredP){
      strcpy(zona_comun->RespC,"Menor");
      printf("Menor\n");
    }

    // Para salir del bucle for en caso de acertar
    if(strcmp(zona_comun->RespC,"Acierto") == 0){
      break;
    }

    // Consumidor libera SEM_1 (SIGNAL)
    //printf("B: libero SEM_1\n");
    operacion.sem_num = SEM_1; // que semaforo (puede haber más de 1!)
    operacion.sem_op = 1; // que operación: +1=signal, -1=wait
    semop(semid,&operacion,1); // envía el comando
    //printf("Proceso B fuera ! (%d)\n",i);

  } // end while

  /* Borrado de la zona de memoria compartida */
  res = shmctl(shmid, IPC_RMID,0);
  if(res==-1){
    perror("shmctl");
    exit(EXIT_FAILURE);
  }

  /* Elimina los semáforos */
  semctl(semid,0,IPC_RMID,0);
  printf("Fin de los semáforos !\n");

  exit(EXIT_SUCCESS);

} // end main
