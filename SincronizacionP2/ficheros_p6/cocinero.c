#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include "defs.h"

#define M 10  // Número de raciones que el cocinero pone en el caldero

int finish = 0;  // Variable global para indicar si el cocinero debe terminar

// Semáforos para sincronización
sem_t *sem_mtx;      // Semáforo que asegura acceso exclusivo a la memoria compartida :)
sem_t *sav_queue;    // Semáforo para la cola de los salvajes
sem_t *cook_queue;   // Semáforo para la cola de los cocineros

shared* shr;         // Puntero a la memoria compartida

// Función que pone raciones en el caldero
void putServingsInPot(int servings)
{
	sem_wait(sem_mtx);  // Bloquea el semáforo para acceso exclusivo a la memoria compartida

	// El cocinero espera si ya hay raciones en el caldero
	while(shr->servings > 0){
		shr->cook_waiting++;  // Aumenta el contador de cocineros esperando
		sem_post(sem_mtx);    // Libera el semáforo para que otros procesos puedan acceder	
		sem_wait(cook_queue);  // Espera a que el cocinero anterior termine
		sem_wait(sem_mtx);     // Vuelve a bloquear el semáforo para acceso exclusivo
	}

	if(finish){
		return;  // Si la variable 'finish' está activada, el cocinero termina
	}

	// Coloca las raciones en el caldero
	shr->servings = servings;
	printf("Cocinero: Poniendo %d raciones en el caldero\n", shr->servings);

	// Despierta a los salvajes esperando
	while (shr->sav_waiting > 0){
		sem_post(sav_queue);  // Despierta a un salvaje
		shr->sav_waiting--;   // Disminuye el contador de salvajes esperando
	}

	sem_post(sem_mtx);  // Libera el semáforo para otros procesos
}

// Función que simula el comportamiento del cocinero
void cook(void)
{
	// El cocinero sigue cocinando hasta que 'finish' sea 1
	while(!finish) {
		putServingsInPot(M);  // Pone raciones en el caldero
	}
}

// Manejador de señales para capturar SIGINT y SIGTERM
void handler(int signo)
{
	finish = 1;  // Cambia el estado a 1 para terminar la ejecución
	printf("Cocinero: Recibida señal %d, terminando...\n", signo);
}

int main(int argc, char *argv[])
{
	// Elimina recursos previos para evitar errores si el programa falla
	shm_unlink(SHM_NAME);
	sem_unlink(SEM_MUTEX);
	sem_unlink(SEM_COOK_QUEUE);
	sem_unlink(SEM_SAV_QUEUE);

	// Crea un archivo de memoria compartida
	int shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
    if (shm_fd == -1) {
        perror("shm_open");
        exit(1);
    }

	// Configura el tamaño de la memoria compartida
    ftruncate(shm_fd, sizeof(shared)); 

	// Mapea la memoria compartida a la dirección del proceso
	shr = mmap(NULL, sizeof(shared), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (shr == MAP_FAILED) {
        perror("mmap");
        exit(1);
    }

	// Inicializa las variables en la memoria compartida
	shr->servings = 0;
	shr->cook_waiting = 0;
	shr->sav_waiting = 0;

	// Configura los manejadores de señales
	struct sigaction sa;
	sa.sa_handler = handler;   // Asigna el manejador
	sa.sa_flags = 0;

	// Configura las señales SIGINT y SIGTERM
	if (sigaction(SIGINT, &sa, NULL) == -1) {
        perror("Error configurando SIGINT");
        exit(1);
    }
    if (sigaction(SIGTERM, &sa, NULL) == -1) {
        perror("Error configurando SIGTERM");
        exit(1);
    }

	// Abre los semáforos para control de acceso
	sem_mtx = sem_open(SEM_MUTEX, O_CREAT, 0644, 1);  // Semáforo de acceso exclusivo
	cook_queue = sem_open(SEM_COOK_QUEUE, O_CREAT, 0644, 0);  // Cola de cocineros
	sav_queue = sem_open(SEM_SAV_QUEUE, O_CREAT, 0644, 0);  // Cola de salvajes
	
	// Verifica si los semáforos se han abierto correctamente
	if (sem_mtx == SEM_FAILED || cook_queue == SEM_FAILED || sav_queue == SEM_FAILED) {
		perror("sem_open failed");
		exit(1);
	}
	
	// Llama a la función que simula el trabajo del cocinero
	cook();

	// Cierra los semáforos y libera los recursos
	if (sem_unlink(SEM_MUTEX) == -1) {
    	perror("sem_unlink of MUTEX failed");
    	return 1;
	}
	if (sem_unlink(SEM_COOK_QUEUE) == -1) {
    	perror("sem_unlink of SEM_COOK_QUEUE failed");
    	return 1;
	}
	if (sem_unlink(SEM_SAV_QUEUE) == -1) {
    	perror("sem_unlink of SEM_SAV_QUEUE failed");
    	return 1;
	}

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

	// Desvincula la memoria compartida
	if (shm_unlink(SHM_NAME) == -1) {
        perror("shm_unlink failed");
        return 1;
    }

	return 0;
} //<3
