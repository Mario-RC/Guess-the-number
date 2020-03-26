/* Creación de una zona de memoria compartida */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

/* Fichero que contiene la información común (clave y estrucura de datos) */
#include "../common.h"

int main(void){

  int shmid;
  datos *zona_comun;
  int res;
  shmid = shmget(CLAVE_SHM, sizeof(datos), IPC_CREAT|0666);
  if(shmid== -1){
    perror("shmget");
    exit(EXIT_FAILURE);
  }

  zona_comun = (datos*) shmat(shmid,0,0);
  printf("El contenido de la memoria común es: %d %g %s\n",
  zona_comun->unEntero,
  zona_comun->unFloat,
  zona_comun->unArray);

  /* Borrado de la zona de memoria compartida */
  res = shmctl(shmid, IPC_RMID,0);
  if(res==-1){
    perror("shmctl");
    exit(EXIT_FAILURE);
  }

  exit(EXIT_SUCCESS);
}
