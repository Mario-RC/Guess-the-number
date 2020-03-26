/* Datos comunes */

/* Clave de acceso a la zona de memoria compartida */
#define CLAVE_SHM ((key_t) 1001)

/* Estructura de datos que se comparte en la zona de memoria común */
typedef struct{

  int unEntero;
  float unFloat;
  char unArray[10];

} datos;
