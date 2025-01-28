/*#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#define CAPACITY 5
#define VIPSTR(vip) ((vip) ? "  vip  " : "not vip")
#define MAX_LENGTH 256

// Definición de la estructura tClientes
struct tClientes {
    int id;
    int vip;
};

// Variables globales
pthread_mutex_t mtx;
pthread_cond_t cond;
int nClientes = 0;
int nClientes_Vip_Espera = 0;
int turn = 0;  // Control de turno

// Funciones para manejar la discoteca
void enter_normal_client(int id) {
    pthread_mutex_lock(&mtx);

    while (nClientes >= CAPACITY || nClientes_Vip_Espera > 0 || turn != id) {
        pthread_cond_wait(&cond, &mtx);
    }

    nClientes++;
    turn++;
    printf("Cliente [%d] normal entró a la discoteca. Aforo: %d\n", id, nClientes);

    pthread_cond_broadcast(&cond);
    pthread_mutex_unlock(&mtx);
}

void enter_vip_client(int id) {
    pthread_mutex_lock(&mtx);
    nClientes_Vip_Espera++;

    while (nClientes >= CAPACITY || turn != id) {
        pthread_cond_wait(&cond, &mtx);
    }

    nClientes++;
    nClientes_Vip_Espera--;
    turn++;
    printf("Cliente [%d] VIP entró a la discoteca. Aforo: %d\n", id, nClientes);

    pthread_cond_broadcast(&cond);
    pthread_mutex_unlock(&mtx);
}

void dance(int id, int isvip) {
    printf("Cliente %2d (%s) está bailando en la discoteca\n", id, VIPSTR(isvip));
    sleep((rand() % 3) + 1);
}

void disco_exit(int id, int isvip) {
    pthread_mutex_lock(&mtx);

    nClientes--;
    printf("Cliente [%d] (%s) salió de la discoteca. Aforo: %d\n", id, VIPSTR(isvip), nClientes);

    pthread_cond_broadcast(&cond);
    pthread_mutex_unlock(&mtx);
}

void *client(void *arg) {
    struct tClientes *clientes = (struct tClientes *)arg;

    int isvip = clientes->vip;
    if (isvip) {
        enter_vip_client(clientes->id);
    } else {
        enter_normal_client(clientes->id);
    }

    dance(clientes->id, isvip);
    disco_exit(clientes->id, isvip);

    free(clientes);  // Liberar memoria dinámica
    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Uso: %s <archivo_entrada>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    FILE *fd = fopen(argv[1], "r");
    if (fd == NULL) {
        perror("Error al abrir el archivo");
        exit(EXIT_FAILURE);
    }

    int M;
    if (fscanf(fd, "%d", &M) != 1) {
        perror("Error al leer el número de clientes");
        fclose(fd);
        exit(EXIT_FAILURE);
    }
	printf("Clientes: %d", M);

    // Inicializar mutex y condición
    pthread_mutex_init(&mtx, NULL);
    pthread_cond_init(&cond, NULL);

    pthread_t th[M];
    printf("M: %d\n", M);

    for (int i = 0; i < M; i++) {
        struct tClientes *clientes = malloc(sizeof(struct tClientes));
        if (clientes == NULL) {
            perror("Error al reservar memoria");
            fclose(fd);
            exit(EXIT_FAILURE);
        }

        clientes->id = i +1;
        if (fscanf(fd, "%d", &clientes->vip) != 1) {
            perror("Error al leer el tipo de cliente");
            free(clientes);
            fclose(fd);
            exit(EXIT_FAILURE);
        }

        if (pthread_create(&th[i], NULL, client, clientes) != 0) {
            perror("Error al crear el hilo");
            free(clientes);
            fclose(fd);
            exit(EXIT_FAILURE);
        }
    }

    for (int i = 0; i < M; i++) {
        pthread_join(th[i], NULL);
    }

    fclose(fd);

    // Destruir mutex y condición
    pthread_mutex_destroy(&mtx);
    pthread_cond_destroy(&cond);

    return 0;
}*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#define CAPACITY 5
#define VIPSTR(vip) ((vip) ? "  vip  " : "no vip")

#define VIP 1
#define NO_VIP 0

struct t_cliente{
int id;
int vip;
};

int clientes_dentro = 0;                 // Clientes actualmente en la discoteca
int clientes_vip_esperando = 0;      // Clientes VIP esperando entrar
int ticket = 0, turn = 0;
int ticket_vip = 0, turn_vip = 0;


pthread_mutex_t mutex;               // Mutex para sincronizar acceso a las variables
pthread_cond_t cond_vip;             // Variable de condición para clientes VIP
pthread_cond_t cond_normal;          // Variable de condición para clientes normales


void enter_normal_client(int id)
{
    int my_turn;
    pthread_mutex_lock(&mutex);
    my_turn = ticket++;
    // Si está lleno o hay VIPS esperando, espera
    while (my_turn != turn || clientes_dentro >= CAPACITY || clientes_vip_esperando > 0)
    {
        pthread_cond_wait(&cond_normal, &mutex);
    }

    clientes_dentro++;
    turn++;
    pthread_cond_broadcast(&cond_normal);
    printf("Cliente %2d (no vip) entra en la discoteca. Clientes dentro: %d\n", id, clientes_dentro);

    pthread_mutex_unlock(&mutex);
}


void enter_vip_client(int id)
{
    int my_turn;
    pthread_mutex_lock(&mutex);
    my_turn = ticket_vip++;
clientes_vip_esperando++;
    // SI está lleno, espera
    while (my_turn != turn_vip || clientes_dentro >= CAPACITY)
    {
        pthread_cond_wait(&cond_vip, &mutex);
    }

    clientes_vip_esperando--;
    clientes_dentro++;
    turn_vip++;
    if (clientes_vip_esperando > 0)
        pthread_cond_broadcast(&cond_vip);
    else
        pthread_cond_broadcast(&cond_normal);
    printf("Cliente %2d (  vip  ) entra en la discoteca. Clientes dentro: %d\n", id, clientes_dentro);

    pthread_mutex_unlock(&mutex);
}


void dance(int id, int isvip)
{
printf("Cliente %2d (%s) bailando en la discoteca \n", id, VIPSTR(isvip));
sleep((rand() % 3) + 1);
}

void disco_exit(int id, int isvip)
{
    pthread_mutex_lock(&mutex);

    clientes_dentro--;
    printf("Cliente %2d (%s) sale de la discoteca. Clientes dentro: %d\n", id, VIPSTR(isvip), clientes_dentro);

     // Si no hay VIPS espernado y hay espacio, se avisa a los no VIPS
    if (clientes_dentro < CAPACITY && clientes_vip_esperando <= 0)
        pthread_cond_broadcast(&cond_normal);
   
    // Si hay VIPS espernado y hay espacio, se avisa a los VIPS
    else if (clientes_dentro < CAPACITY)
        pthread_cond_broadcast(&cond_vip);

pthread_mutex_unlock(&mutex);
   
}


void *client(void *arg)
{
    struct t_cliente *client = (struct t_cliente *) arg;

    if (client->vip)
        enter_vip_client(client->id);
   
    else
        enter_normal_client(client->id);

    dance(client->id, client->vip);
    disco_exit(client->id, client->vip);

    free(client);
    pthread_exit(NULL);
}


int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <input_file>\n", argv[0]);
        return EXIT_FAILURE;
    }

    // Abrir el fichero
    FILE *file = fopen(argv[1], "r");
    if (!file)
    {
        perror("Error opening file");
        return EXIT_FAILURE;
    }

    int M;
    if (fscanf(file, "%d", &M) != 1){
fprintf(stderr, "Error al leer el numero clientes");
exit(EXIT_FAILURE);
}

int *clientes = malloc(sizeof(int) * M);

    // Crear hilos
    for (int i = 0; i < M; i++){
        if(fscanf(file, "%d", &clientes[i]) != 1){
fprintf(stderr, "Error al leer el cliente");
fclose(file);
exit(EXIT_FAILURE);
}
    }

    fclose(file);

// Inicializar mutex y variables de condición
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond_vip, NULL);
    pthread_cond_init(&cond_normal, NULL);

//Creación de hilos
pthread_t *threads = malloc(sizeof(pthread_t) * M);

for(int i = 0; i < M; i++){
struct t_cliente *c = malloc(sizeof(c));
c->id = i + 1;
c->vip = clientes[i];
pthread_create(&threads[i], NULL, client, c);
}

    //LIbera la memoria asociada al array de clientes (ya no se usa)
free(clientes);

    // Espera a que termine todos los hilos
    for (int i = 0; i < M; i++)
        pthread_join(threads[i], NULL);

free(threads);

    // Destruir mutex y variables de condición
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond_vip);
    pthread_cond_destroy(&cond_normal);

    return EXIT_SUCCESS;
}
