/* Creación de una zona de memoria compartida */

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

  int shmid,semid;
  datos *zona_comun;
  struct sembuf operacion;
  int i = 100;
  int predPMax = 100;
  int predPmin = 0;
  int predPuno = 0;

  srandom(getpid()); // Inicializa la semilla para los numeros casuales

  /* Creación de la zona de memoria compartida */
  shmid = shmget(CLAVE_SHM, sizeof(datos), IPC_CREAT|0666);
  if(shmid== -1){
    perror("shmget");
    exit(EXIT_FAILURE);
  }

  zona_comun = (datos*) shmat(shmid,0,0);
  zona_comun->PredP = (int)(100.0*random()/RAND_MAX); // Predicción inicial del padre, será un numero entre 0 y 256

  // Creación de los semáforos
  semid = semget(CLAVE,2,IPC_CREAT|0600);
  if(semid == -1){
    perror("semget");
    exit(EXIT_FAILURE);
  }

  /* Inicialización del semáforo 1 con valor 1 */
  semctl(semid,SEM_1,SETVAL,1);
  printf("Valor semáforo 1: %d\n", semctl(semid,SEM_1,GETVAL,0));
  operacion.sem_flg = 0; /* No se activa ninguna opcion */

  /* Inicialización del semáforo 2 con valor 0 */ 
  semctl(semid,SEM_2,SETVAL,0);
  printf("Valor semáforo 2: %d\n\n", semctl(semid,SEM_2,GETVAL,0));
  operacion.sem_flg = 0; /* No se activa ninguna opcion */

  // Inicio del número de mensajes a mandar
  while(i > 0){

    i--;

    // Productor espera SEM_1 (WAIT)
    //printf("A: espero SEM_1 (WAIT)\n");
    operacion.sem_num = SEM_1; // que semaforo (puede haber más de 1!)
    operacion.sem_op = -1; // que operación: +1=signal, -1=wait
    semop(semid,&operacion,1); // envía el comando
    //printf("Proceso A dentro ! (%d)\n",i);

    if(strcmp(zona_comun->RespC,"Acierto") == 0){ // Acierto
      printf("Productor: Lo he conseguido!!\n\n");
      break;
    }
    else if(strcmp(zona_comun->RespC,"Mayor") == 0){ // Mayor respC = 1
      predPmin = zona_comun->PredP;
      zona_comun->PredP = predPMax - ((predPMax-zona_comun->PredP)/2);
    }
    else if(strcmp(zona_comun->RespC,"Menor") == 0){ // Menor respC = 2
      predPMax = zona_comun->PredP;
      zona_comun->PredP = zona_comun->PredP - ((zona_comun->PredP-predPmin)/2);
      if(zona_comun->PredP == 1){
        predPuno++;
        if(predPuno == 2)
          zona_comun->PredP = 0;
      }
    }

    printf("Pred: %d\n",zona_comun->PredP);

    // Productor libera SEM_2 (SIGNAL)
    //printf("A: libero SEM_2 (SIGNAL)\n");
    operacion.sem_num = SEM_2;
    operacion.sem_op = 1; // que operación: +1=signal, -1=wait
    semop(semid,&operacion,1); // envía el comando
    //printf("Proceso A fuera ! (%d)\n",i);

  } // end while

  /* Elimina los semáforos */
  semctl(semid,0,IPC_RMID,0);
  printf("Fin de los semáforos!\n");

  exit(EXIT_SUCCESS);

} // end main
