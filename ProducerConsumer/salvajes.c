#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <semaphore.h>

#define NUMITER 3
#define SHARED_MEM_NAME "/caldero"
#define SEM_EMPTY_NAME "/sem_empty"
#define SEM_FULL_NAME "/sem_full"

int *caldero;
sem_t *sem_empty;
sem_t *sem_full;

int getServingsFromPot(void)
{
	int result;
	int restantes;
	printf("Salvaje %d esperando porciones\n", getpid());
	sem_wait(sem_full);
	if(*caldero > 0) {
		(*caldero)--;
		restantes = *caldero;
		result = 0;
		printf("El salvaje %d está comiendo una porción. Restantes: %d\n", getpid(), restantes);
	} else {
		result = 1;
	}

	if(*caldero == 0) {
		sem_post(sem_empty);
	} else {
		sem_post(sem_full);
	}

	return result;
}

void eat(void)
{
	unsigned long id = (unsigned long) getpid();
	printf("Savage %lu eating\n", id);
	sleep(rand() % 5);
}

void savages(void)
{
	for(int i = 0; i < NUMITER; i++) {
		if(getServingsFromPot()) {
			eat();
		}
	}
}

int main(int argc, char *argv[])
{
	int fd = shm_open(SHARED_MEM_NAME, O_RDWR, 0725);
	if(fd == -1) {
		perror("shm_open");
		return 1;
	}

	caldero = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if(caldero == MAP_FAILED) {
		perror("mmap");
		return 1;
	}

	sem_empty = sem_open(SEM_EMPTY_NAME, 0);
	sem_full = sem_open(SEM_FULL_NAME, 0);

	if(sem_empty == SEM_FAILED || sem_full == SEM_FAILED) {
		perror("sem_open");
		return 1;
	}

	savages();

	munmap(caldero, sizeof(int));
	close(fd);
	sem_close(sem_empty);
	sem_close(sem_full);

	return 0;
}
