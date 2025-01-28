#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "defs.h"

#define NUMITER 11  // Número de iteraciones para el comportamiento de los salvajes :)

shared* shr;  // Puntero a la memoria compartida

sem_t *sem_mtx;      // Semáforo de acceso exclusivo a la memoria compartida
sem_t *cook_queue;   // Semáforo para la cola de los cocineros
sem_t *sav_queue;    // Semáforo para la cola de los salvajes

// Función que obtiene raciones del caldero
int getServingsFromPot(void)
{
	sem_wait(sem_mtx);  // Bloquea el semáforo para acceso exclusivo

	// Si el caldero está vacío, avisa al cocinero y espera
	while(shr->servings == 0){
		if(shr->cook_waiting > 0){
			sem_post(cook_queue);  // Despierta al cocinero si está esperando
			shr->cook_waiting--;   // Disminuye el número de cocineros esperando
		}
		shr->sav_waiting++;   // Aumenta el número de salvajes esperando
		printf("Salvaje: El caldero está vacío, avisando al cocinero...\n");
		sem_post(sem_mtx);    // Libera el semáforo para otros procesos
		sem_wait(sav_queue);   // Espera que el cocinero reponga las raciones
		sem_wait(sem_mtx);     // Vuelve a bloquear el semáforo
	}

	// Sirve una ración
	shr->servings--;  
	printf("Salvaje %lu: Sirviéndose una ración, quedan %d raciones.\n", (unsigned long)getpid(), shr->servings);
	sem_post(sem_mtx);  // Libera el semáforo
	return 1;
}

// Función que simula el proceso de comer
void eat(void)
{
	unsigned long id = (unsigned long) getpid();
	printf("Savage %lu eating\n", id);
	sleep(rand() % 5);  // Simula el tiempo de comer
}

// Función que simula el comportamiento de los salvajes
void savages(void)
{
	// Cada salvaje realiza el ciclo de obtener una ración y comer
	for (int i = 0; i < NUMITER; i++){
  		getServingsFromPot();
  		eat();
	}
}

int main(int argc, char *argv[])
{	
	// Abre la memoria compartida
	int shm_fd = shm_open(SHM_NAME, O_RDWR, S_IRUSR | S_IWUSR);
    if (shm_fd == -1) {
        perror("shm_open");
        exit(1);
    }

	// Mapea la memoria compartida
    shr = mmap(NULL, sizeof(shared), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (shr == MAP_FAILED) {
        perror("mmap");
        exit(1);
    }

	// Abre los semáforos
	sem_mtx = sem_open(SEM_MUTEX, 1);  // Inicializado en 0 (empieza cocinando)
	cook_queue = sem_open(SEM_COOK_QUEUE, 0);  // Inicializado en 0 (caldero vacío)
	sav_queue = sem_open(SEM_SAV_QUEUE, O_CREAT, 0644, 0);  // Inicializado en 0 (no está comiendo)

	// Verifica si los semáforos se han abierto correctamente
	if (sem_mtx == SEM_FAILED || cook_queue == SEM_FAILED || sav_queue == SEM_FAILED) {
		perror("sem_open failed");
		exit(1);
	}

	// Simula el comportamiento de los salvajes
	savages();

	// Cierra los semáforos
	if (sem_close(sem_mtx) == -1) {
        perror("sem_close  of sem_mtx failed");
        return 1;
    }
	if (sem_close(cook_queue) == -1) {
        perror("sem_close of cook_queue failed");
        return 1;
    }
	if (sem_close(sav_queue) == -1) {
        perror("sem_close of sav_queue failed");
        return 1;
    }

	// Desmapea la memoria compartida
	if (munmap(shr, sizeof(int)) == -1) {
        perror("munmap failed");
        return 1;
    }

	// Muestra un mensaje al terminar
	unsigned long id = (unsigned long) getpid();
	printf("Savage %lu has finished eating\n", id);
	exit(0);  // Termina el proceso del salvaje <3
	return 0;
}


// for i in {1..5}; do ./salvajes & done

