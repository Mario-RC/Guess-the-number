// Compilación:
// gcc <archivo> -lpthread

#include<stdio.h>
#include<string.h>
#include<pthread.h>
#include<stdlib.h>
#include<unistd.h>

int una_variable_global = -1;
	
void* function1(void *arg){

  int i = 0;
  pthread_t my_id = pthread_self();

  for (i=0;i<5;i++){

    una_variable_global = i;
    printf("\n Hello, soy el thread 1 (%lu) y v.g. vale %d\n",(unsigned long) my_id,una_variable_global);
    sleep(1);

  } // end for

  printf("\n T1 says: bye bye !\n");
  return NULL;

} // end void* function1(void *arg)

void* function2(void *arg){

  pthread_t my_id = pthread_self();
  int i;
  float *la_x;
  la_x = (float*)arg;

  for (i=0;i<8;i++){

    una_variable_global -= 3;
    printf("\n Hello, soy el thread 2 (%lu) y v.g. vale %d\n",(unsigned long) my_id,una_variable_global);
    sleep(1);

  } // end for

  *la_x = 0.456;
  printf("\n T2 says: Hasta luego lucas !\n");
  return NULL;

} // end void* function2(void *arg)

int main(void){

  pthread_t t1_id,t2_id;
  int i,err;
  float x = 0.123;

  err = pthread_create(&t1_id, NULL, &function1, NULL);
  if (err != 0)
    printf("\nCan't create thread :[%s]", strerror(err));
  else
    printf("\n Thread T1 created successfully\n");

  err = pthread_create(&t2_id, NULL, &function2, &x);
  if (err != 0)
    printf("\nCan't create thread :[%s]", strerror(err));
  else
    printf("\n Thread T2 created successfully\n");

  for (i=0;i<15;i++){

    printf("\n Hello, soy el proceso principal: x vale %4.3f y la var. global vale %d\n",x,una_variable_global);
    sleep(1);

  } // end for

  return 0;

} // end int main(void)
