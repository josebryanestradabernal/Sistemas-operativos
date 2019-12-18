#include <sys/mman.h>
#include <stdio.h>
#include <semaphore.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>

#define MAX_BUFFER 10 /* tamaño del buffer */
#define DATOS_A_PRODUCIR 100 /* datos a producir */

void productor(void);

sem_t *huecos;
sem_t *elementos;
int *buffer; /* puntero al buffer de números enteros */

int main(void){
int shd;

/* se crean e inician semáforos */
huecos = sem_open("HUECOS", O_CREAT ,00666, 10);
elementos = sem_open("ELEMENTOS", O_CREAT, 00666, 0);
if (huecos == SEM_FAILED || elementos == SEM_FAILED) {
perror("Error en sem_open");
exit(1);
}

/* se crea el segmento de memoria compartida utilizado como
buffer circular */
shd = open("BUFFER",O_CREAT|O_TRUNC|O_RDWR,00666);
if (shd == -1) {
perror("Error en open");
exit(1);
}
ftruncate(shd,(MAX_BUFFER*sizeof(int)));
buffer = mmap(NULL,(MAX_BUFFER*sizeof(int)) , PROT_WRITE, MAP_SHARED,shd,0);
if (buffer == MAP_FAILED) {
perror("Error en mmap");
exit(1);
}
productor(); /* se ejecuta el código del productor */

/* se desproyecta el buffer */
munmap(buffer, MAX_BUFFER*sizeof(int));
close (shd);
unlink("BUFFER");

/* cierran y se destruyen los semáforos */
sem_close(huecos);
sem_close(elementos);
sem_unlink("HUECOS");
sem_unlink("ELEMENTOS");

return 0;
}


/* código del proceso productor */
void productor(void) {
int dato; /* dato a producir */
int posicion = 0; /* posición donde insertar el elemento*/
int j;

for (j=0; j<DATOS_A_PRODUCIR; j++) {
dato = j;
printf("Produce %d\n", dato);
sleep (1);
sem_trywait(huecos); /* un hueco menos */
buffer[posicion]=dato;
posicion=(posicion+1) % MAX_BUFFER; /* nueva posición */
sem_post(elementos); /* un elemento más);*/
}
return;
}

