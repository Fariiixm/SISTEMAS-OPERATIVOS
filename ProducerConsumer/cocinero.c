#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <semaphore.h>


#define M 10
#define SHARED_MEM_NAME "/caldero"
#define SEM_EMPTY_NAME "/sem_empty"
#define SEM_FULL_NAME "/sem_full"

int finish = 0;
int *caldero;
sem_t *sem_empty;
sem_t *sem_full;

void putServingsInPot(int servings)
{
	*caldero = servings;
	printf("LLenando el caldero con %d raciones\n", servings);
}

void cook(void)
{
	while(!finish) {
		printf("Esperando a que no queden mas raciones\n");
		sem_wait(sem_empty);
		if(finish) break;
		putServingsInPot(M);
		sem_post(sem_full);
	}
}

void handler(int signo)
{
	finish = 1;
}

int main(int argc, char *argv[])
{
	struct sigaction sa;
	sa.sa_handler = handler;
	sa.sa_flags = 0;
	sigemptyset(&sa.sa_mask);

	sigaction(SIGINT, &sa, NULL);
	sigaction(SIGTERM, &sa, NULL);

	// creacion memoria compartida
	int fd = shm_open(SHARED_MEM_NAME, O_CREAT | O_RDWR, 0777);
	if(fd == -1) {
		perror("shm_open");
		return 1;
	}

	if(ftruncate(fd, sizeof(int)) == -1) {
		perror("ftruncate");
		return 1;
	}

	caldero = mmap(0, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if(caldero == MAP_FAILED) {
		perror("mmap");
		return 1;
	}

	//creacion semaforos
	sem_empty = sem_open(SEM_EMPTY_NAME, O_CREAT, 0666, 1); // inicializamos a 1 porque empieza vacio
	sem_full = sem_open(SEM_FULL_NAME, O_CREAT, 0666, 0); // inicializamos a 0 porque no está lleno todavia

	if(sem_empty == SEM_FAILED || sem_full == SEM_FAILED) {
		perror("sem_open");
		return 1;
	}

	cook();

	munmap(caldero, sizeof(int));
	shm_unlink(SHARED_MEM_NAME);
	sem_close(sem_empty);
	sem_close(sem_full);
	sem_unlink(SEM_EMPTY_NAME);
	sem_unlink(SEM_FULL_NAME);

	return 0;
}
