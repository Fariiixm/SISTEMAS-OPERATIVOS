#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

struct tInfo {
    int id;
    char prioridad;
};

void *thread_usuario(void *arg) {
    struct tInfo *info = (struct tInfo *)arg;
    pthread_t idsys = pthread_self();

    printf("[%lu] thread id = %d, con prioridad %c\n", idsys, info->id, info->prioridad);
    free(info);

    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("usage: %s <numThreads>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int numThreads = atoi(argv[1]);
    if (numThreads <= 0) {
        printf("error, numThreads > 0\n");
        exit(EXIT_FAILURE);
    }

    pthread_t th[numThreads];

    for (int i = 0; i < numThreads; ++i) {
        struct tInfo *info = malloc(sizeof(*info));
        if (info == NULL) {
            perror("malloc");
            exit(EXIT_FAILURE);
        }

        info->id = i + 1;
        info->prioridad = (i % 2 == 0) ? 'P' : 'N';

        if (pthread_create(&th[i], NULL, thread_usuario, info) != 0) {
            perror("pthread_create");
            free(info);
            exit(EXIT_FAILURE);
        }
    }

    for (int i = 0; i < numThreads; ++i) {
        pthread_join(th[i], NULL);
    }

    return 0;
}
//./hilos 6