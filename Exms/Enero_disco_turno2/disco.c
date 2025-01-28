#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#define CAPACITY 3
#define VIPSTR(prof) ((prof) ? "professor" : "student")

#define VIP 1
#define NO_VIP 0

struct t_cliente{
int id;
int prof;
};

int clientes_dentro = 0;                 // Clientes actualmente en la discoteca
int clientes_vip_esperando = 0;      // Clientes VIP esperando entrar
int ticket = 0, turn = 0;
int ticket_vip = 0, turn_vip = 0;

int vacaciones = 0;

pthread_mutex_t mutex;               // Mutex para sincronizar acceso a las variables
pthread_cond_t cond_vip;             // Variable de condición para clientes VIP
pthread_cond_t cond_normal;          // Variable de condición para clientes normales


void enter_normal_client(int id)
{
    int my_turn;
    pthread_mutex_lock(&mutex);
    my_turn = ticket++;
    printf("User %2d (student) waiting on the queue. Aforo: %d\n", id, clientes_dentro);
    // Si está lleno o hay VIPS esperando, espera
    while (my_turn != turn || clientes_dentro >= CAPACITY || clientes_vip_esperando > 0 || vacaciones)
    {
        pthread_cond_wait(&cond_normal, &mutex);
    }

    clientes_dentro++;
    turn++;
    pthread_cond_broadcast(&cond_normal);
    

    pthread_mutex_unlock(&mutex);
}


void enter_vip_client(int id)
{
    int my_turn;
    pthread_mutex_lock(&mutex);
    my_turn = ticket_vip++;
clientes_vip_esperando++;
    // SI está lleno, espera
    printf("User %2d ( professor ) waiting on the queue. Aforo: %d\n", id, clientes_dentro);
    while (my_turn != turn_vip || clientes_dentro >= CAPACITY || vacaciones)
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

    pthread_mutex_unlock(&mutex);
}


void dance(int id, int isvip)
{
//printf("Cliente %2d (%s) bailando en la discoteca \n", id, VIPSTR(isvip));
//sleep((rand() % 3) + 1);
printf("User %2d (%s) is reading books for 2 seconds. Aforo: %d\n", id, VIPSTR(isvip), clientes_dentro);
sleep(2);
}

void disco_exit(int id, int isvip)
{
    pthread_mutex_lock(&mutex);

    clientes_dentro--;
    printf("Cliente %2d (%s) leaves the library. Aforo: %d\n", id, VIPSTR(isvip), clientes_dentro);

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

    if (client->prof)
        enter_vip_client(client->id);
   
    else
        enter_normal_client(client->id);

    dance(client->id, client->prof);
    disco_exit(client->id, client->prof);

    free(client);
    pthread_exit(NULL);
}

void *vacacionesProcess(void *arg){
    //pthread_mutex_lock(&mutex);
    while(1){
        sleep(4); // Biblioteca cierra tras 4 segundos

        pthread_mutex_lock(&mutex);
        vacaciones = 1;
        printf("Library is closing for vacation.\n");
        pthread_mutex_unlock(&mutex);

        sleep(2); // Biblioteca cerrada por 2 segundos

        pthread_mutex_lock(&mutex);
        vacaciones = 0;
        printf("Library is now open after vacation.\n");
        pthread_mutex_unlock(&mutex);
    }
    //pthread_mutex_unlock(&mutex);

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
pthread_t vacaciones;
pthread_create(&vacaciones, NULL, vacacionesProcess, NULL);

for(int i = 0; i < M; i++){
    struct t_cliente *c = malloc(sizeof(c));
    c->id = i + 1;
    c->prof = clientes[i];
    pthread_create(&threads[i], NULL, client, c);
}


    //LIbera la memoria asociada al array de clientes (ya no se usa)
free(clientes);

    // Espera a que termine todos los hilos
    for (int i = 0; i < M; i++){
        pthread_join(threads[i], NULL);
        printf("salio el : %d\n", i);
    }
    pthread_cancel(vacaciones);

free(threads);

    // Destruir mutex y variables de condición
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond_vip);
    pthread_cond_destroy(&cond_normal);

    return EXIT_SUCCESS;
}
