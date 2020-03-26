// Compilación:
// gcc <archivo> -lpthread

#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

int una_variable_global = -1;
pthread_mutex_t mut;
pthread_cond_t cond;

void* function1(void *arg){

  int i = 0;
  printf("\n Hello, soy el thread 1 y me voy a dormir...\n");

  // Pide acceso al mutex
  pthread_mutex_lock(&mut);

  // Pone el thread a la espera (suspenso) de condition variable
  pthread_cond_wait(&cond, &mut);

  for (i=0;i<3;i++){

    printf("\n Hello, soy el thread 1\n");
    sleep(1);

  } // end for

  printf("\n T1 says: bye bye !\n");
  pthread_exit(0);
  //return NULL;

} // end void* function1(void *arg)

void* function2(void *arg){

  int i;

  for (i=0;i<10;i++){

    printf("\n Hello, soy el thread 2\n" );
    sleep(1);

    if(i==3){
	
      // Despierta el thread a la espera de condition variable
      pthread_cond_signal(&cond);

      // Libera el mutex
      pthread_mutex_unlock(&mut);

    } // end if

  } // end for

  printf("\n T2 says: Hasta luego lucas !\n");
  pthread_exit(0);
  //return NULL;

} // end void* function2(void *arg)

int main(void){

  pthread_t t1_id,t2_id;
  int i,err;

  // Inicio del mutex y la condition variable
  pthread_cond_init(&cond,NULL);
  pthread_mutex_init(&mut,NULL);

  err = pthread_create(&t1_id, NULL, &function1, NULL);
  if (err != 0)
    printf("\nCan't create thread :[%s]", strerror(err));
  else
    printf("\n Thread T1 created successfully\n");

  err = pthread_create(&t2_id, NULL, &function2, NULL);
  if (err != 0)
    printf("\nCan't create thread :[%s]", strerror(err));
  else
    printf("\n Thread T2 created successfully\n");

  for (i=0;i<15;i++){

    printf("\n Hello, soy el proceso principal\n");
    sleep(1);

  } // end for

  /* Esperar terminación */
  pthread_join(t1_id, NULL);
  pthread_join(t2_id, NULL);

  /* Destruir el mutex y la condition variable */
  pthread_mutex_destroy(&mut);
  pthread_cond_destroy(&cond);

  return 0;

} // end int main(void)
