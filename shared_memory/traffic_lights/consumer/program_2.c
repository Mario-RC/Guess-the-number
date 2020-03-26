/* Uso de semaforos y ciclos de espera, programa 2 */
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

  // el proceso 2 NO inicializa el semaforo !
  printf("Valor semáforo: %d\n", semctl(semid,SEM_1,GETVAL,0));
  operacion.sem_flg=0; /* No activamos ninguna opcion */
  i=10;

  while(i>0){

    i--;
    printf("B: espero sem1\n");
    operacion.sem_num = SEM_1; // que semaforo (puede haber más de 1!)
    operacion.sem_op = -1; // que operación: +1=signal, -1=wait
    semop(semid,&operacion,1); // envía el comando
    printf("Proceso B dentro ! (%d)\n",i);

    pausa(); // simula la actividad en la region critica

    printf("B: libero sem1\n");
    operacion.sem_num = SEM_1; // que semaforo (puede haber más de 1!)
    operacion.sem_op = 1; // que operación: +1=signal, -1=wait
    semop(semid,&operacion,1); // envía el comando
    printf("Proceso B fuera ! (%d)\n",i);

    pausa(); // simula la actividad fuera de la region critica
  }

  /* Elimina el semáforo */
  semctl(semid,0,IPC_RMID,0);
  printf("Fin !\n");
}
