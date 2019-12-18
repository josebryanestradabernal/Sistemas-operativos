#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <fcntl.h>


#define MAX_BUFFER 10 /* tamaño del buffer */
#define DATOS_A_PRODUCIR 100 /* datos a producir */

void consumidor(void);

sem_t *huecos;
sem_t *elementos;
int *buffer; /* buffer de números enteros */

int main(){
int shd;

/* se abren los semáforos */
huecos = sem_open("HUECOS", O_CREAT ,00666, 10);
elementos = sem_open("ELEMENTOS", O_CREAT, 00666, 0);
if (huecos == SEM_FAILED || elementos == SEM_FAILED) {
perror("Error en sem_open");
exit(1);
}
/* se abre el segmento de memoria compartida utilizado como
buffer circular */
shd = open("BUFFER",O_CREAT|O_TRUNC|O_RDWR,00666);
if (shd == -1) {
perror("Error en open");
exit(0);
}
buffer = (int *)mmap(NULL, MAX_BUFFER*sizeof(int),
PROT_READ,MAP_SHARED, shd, 0);
if (buffer == NULL) {
perror("Error en mmap");
exit(1);
}

consumidor(); /* se ejecuta el código del consumidor */

/* se desproyecta el buffer */
munmap(buffer, MAX_BUFFER*sizeof(int));
close(shd);

/* se cierran semáforos */
sem_close(huecos);
sem_close(elementos);
exit(0);
}


/* código del proceso productor */
void consumidor(void){
int dato; /* dato a consumir */
int posicion = 0; /* posición que indica el elemento a extraer */
int j;

for (j=0; j<DATOS_A_PRODUCIR; j++) {
dato = j;
sem_wait(elementos); /* un elemento menos */
dato = buffer[posicion];
printf("Consume %d\n", dato);
posicion=(posicion+1) % MAX_BUFFER; /* nueva posición */
sem_post(huecos); /* un hueco más); */
}
return;
}
