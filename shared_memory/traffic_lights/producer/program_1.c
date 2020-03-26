/* Creación de un semaforo y ciclos de espera */
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <stdlib.h>
#include <unistd.h>
#include "../common.h"

int main(void){

  int i;
  int semid;
  struct sembuf operacion;

  // inicializa la semilla de los numeros casuales
  srandom(getpid());

  // creacción del semaforo
  semid = semget(CLAVE,1,IPC_CREAT|0600);
  if(semid == -1){
    perror("semget");
    exit(EXIT_FAILURE);
  }

  /* Inicialización del semáforo */
  semctl(semid,SEM_1,SETVAL,1);
  printf("Valor semáforo: %d\n", semctl(semid,SEM_1,GETVAL,0));
  operacion.sem_flg = 0; /* No se activa ninguna opcion */
  i=10;

  while(i>0){
    i--;
    printf("A: espero sem1 (WAIT)\n");
    operacion.sem_num = SEM_1; // que semaforo (puede haber más de 1!)
    operacion.sem_op = -1; // que operación: +1=signal, -1=wait
    semop(semid,&operacion,1); // envía el comando
    printf("Proceso A dentro ! (%d)\n",i);

    pausa(); // simula la actividad en la region critica

    printf("A: libero sem1 (SIGNAL)\n");
    operacion.sem_num = SEM_1;
    operacion.sem_op = 1; // que operación: +1=signal, -1=wait
    semop(semid,&operacion,1); // envía el comando
    printf("Proceso A fuera ! (%d)\n",i);

    pausa(); // simula la actividad fuera de la region critica
  }

  /* Elimina el semáforo */
  semctl(semid,0,IPC_RMID,0);
  printf("Fin !\n");
}
