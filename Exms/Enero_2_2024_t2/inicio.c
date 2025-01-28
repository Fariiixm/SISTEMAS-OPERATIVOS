#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>

int fd1;
char buffer[5];  // Espacio para 4 caracteres más el terminador nulo
int current_id = 1;  // Controla el turno de los hilos
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

void *thread_usuario(void *arg) {
    int *id = (int *)arg;

    pthread_mutex_lock(&mutex);

    // Esperar hasta que sea el turno del hilo actual
    while (*id != current_id) {
        pthread_cond_wait(&cond, &mutex);
    }

    // Escribir exactamente 4 veces el número del hilo
    sprintf(buffer, "%04d", (*id - 1) * 1111);

    lseek(fd1, 0, SEEK_END);  // Mover al final del archivo
    write(fd1, buffer, 4);    // Escribir los 4 caracteres

    // Avanzar al siguiente hilo
    current_id++;
    pthread_cond_broadcast(&cond);  // Notificar a los otros hilos

    pthread_mutex_unlock(&mutex);

    free(id);
    return NULL;
}

int main(void) {
    // Abrir el archivo de salida
    fd1 = open("output.txt", O_CREAT | O_TRUNC | O_RDWR, S_IRUSR | S_IWUSR);
    if (fd1 == -1) {
        perror("Error abriendo el archivo");
        exit(EXIT_FAILURE);
    }

    // Crear los hilos
    pthread_t th[10];
    for (int i = 0; i < 10; i++) {
        int *id = malloc(sizeof(*id));
        *id = i + 1;

        if (pthread_create(&th[i], NULL, thread_usuario, id) != 0) {
            perror("pthread_create");
            free(id);
            exit(EXIT_FAILURE);
        }
    }

    // Esperar a que terminen los hilos
    for (int i = 0; i < 10; i++) {
        pthread_join(th[i], NULL);
    }

    // Leer y mostrar el contenido del archivo
    lseek(fd1, 0, SEEK_SET);  // Reposicionar el puntero al inicio del archivo
    printf("File contents are:\n");
    char c;
    while (read(fd1, &c, 1) > 0)
        printf("%c", c);  // Imprimir cada caracter leído
    printf("\n");

    close(fd1);  // Cerrar el archivo
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond);

    return 0;
}
