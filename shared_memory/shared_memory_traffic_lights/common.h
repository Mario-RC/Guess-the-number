/* Datos comunes */

/* Clave de acceso a la zona de memoria compartida */
#define CLAVE_SHM ((key_t) 1001)

/* Definición de los nombres de los semáforos */
#define SEM_1 0
#define SEM_2 1
#define CLAVE 1

/* Estructura de datos que se comparte en la zona de memoria común */
typedef struct{

  int PredP;
  char RespC[10];

} datos;

// Función que implementa una pausa de duracción casual entre 1 y 3 segundos
void pausa(){

  int pausa;
  pausa = 1000+(int)(2000*(random()/(float)RAND_MAX)); // entre 1000 y 3000 microsegundos
  usleep(pausa*1000); // entre 1000 y 3000 millisegundos

}
