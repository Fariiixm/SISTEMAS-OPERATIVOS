#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#define CAPACITY 5

#define VIPSTR(vip) ((vip) == 0 ? "normal" : (vip) == 1 ? "vip" : "special")

struct t_cliente {
    int id;
    int vip;
};

// Variables globales
int clientes_dentro = 0;
int clientes_vip_esperando = 0;
int clientes_normales_esperando = 0;
int clientes_especiales_esperando = 0;

pthread_mutex_t mutex;
pthread_cond_t cond_normal;
pthread_cond_t cond_vip;
pthread_cond_t cond_especial;

// Funciones para gestionar la entrada
void enter_normal_client(int id) {
    pthread_mutex_lock(&mutex);
    clientes_normales_esperando++;

    while (clientes_dentro >= CAPACITY || clientes_vip_esperando > 0 || clientes_especiales_esperando > 0) {
        pthread_cond_wait(&cond_normal, &mutex);
    }

    clientes_normales_esperando--;
    clientes_dentro++;
    printf("Cliente %2d (%s) entra en la discoteca. Clientes dentro: %d\n", id, VIPSTR(0), clientes_dentro);

    pthread_mutex_unlock(&mutex);
}

void enter_vip_client(int id) {
    pthread_mutex_lock(&mutex);
    clientes_vip_esperando++;

    while (clientes_dentro >= CAPACITY || clientes_especiales_esperando > 0) {
        pthread_cond_wait(&cond_vip, &mutex);
    }

    clientes_vip_esperando--;
    clientes_dentro++;
    printf("Cliente %2d (%s) entra en la discoteca. Clientes dentro: %d\n", id, VIPSTR(1), clientes_dentro);

    pthread_mutex_unlock(&mutex);
}

void enter_special_client(int id) {
    pthread_mutex_lock(&mutex);
    clientes_especiales_esperando++;

    while (clientes_dentro > 0) {
        pthread_cond_wait(&cond_especial, &mutex);
    }

    clientes_especiales_esperando--;
    clientes_dentro++;
    printf("Cliente %2d (%s) entra en la discoteca. Clientes dentro: %d\n", id, VIPSTR(2), clientes_dentro);

    pthread_mutex_unlock(&mutex);
}

// Función para bailar
void dance(int id, int vip) {
    printf("Cliente %2d (%s) bailando en la discoteca\n", id, VIPSTR(vip));
    sleep((rand() % 3) + 1);
}

// Función para salir
void disco_exit(int id, int vip) {
    pthread_mutex_lock(&mutex);

    clientes_dentro--;
    printf("Cliente %2d (%s) sale de la discoteca. Clientes dentro: %d\n", id, VIPSTR(vip), clientes_dentro);

    if (clientes_dentro == 0 && clientes_especiales_esperando > 0) {
        pthread_cond_broadcast(&cond_especial);
    } else if (clientes_dentro < CAPACITY && clientes_vip_esperando > 0) {
        pthread_cond_broadcast(&cond_vip);
    } else if (clientes_dentro < CAPACITY) {
        pthread_cond_broadcast(&cond_normal);
    }

    pthread_mutex_unlock(&mutex);
}

// Función ejecutada por cada hilo
void *client(void *arg) {
    struct t_cliente *cliente = (struct t_cliente *)arg;

    if (cliente->vip == 0) {
        enter_normal_client(cliente->id);
    } else if (cliente->vip == 1) {
        enter_vip_client(cliente->id);
    } else {
        enter_special_client(cliente->id);
    }

    dance(cliente->id, cliente->vip);
    disco_exit(cliente->id, cliente->vip);

    free(cliente);
    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Uso: %s <archivo_entrada>\n", argv[0]);
        return EXIT_FAILURE;
    }

    FILE *file = fopen(argv[1], "r");
    if (!file) {
        perror("Error al abrir el archivo");
        return EXIT_FAILURE;
    }

    int M;
    fscanf(file, "%d", &M);

    pthread_t threads[M];
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond_normal, NULL);
    pthread_cond_init(&cond_vip, NULL);
    pthread_cond_init(&cond_especial, NULL);

    for (int i = 0; i < M; i++) {
        struct t_cliente *cliente = malloc(sizeof(struct t_cliente));
        if (!cliente) {
            perror("Error al reservar memoria");
            exit(EXIT_FAILURE);
        }

        cliente->id = i + 1;
        fscanf(file, "%d", &cliente->vip);

        if (pthread_create(&threads[i], NULL, client, cliente) != 0) {
            perror("Error al crear el hilo");
            exit(EXIT_FAILURE);
        }
    }

    fclose(file);

    for (int i = 0; i < M; i++) {
        pthread_join(threads[i], NULL);
    }

    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond_normal);
    pthread_cond_destroy(&cond_vip);
    pthread_cond_destroy(&cond_especial);

    return EXIT_SUCCESS;
}
