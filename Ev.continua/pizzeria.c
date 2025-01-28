#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define N 5 
#define PIZZASTR(tipoPizza) ((tipoPizza) == 0 ? "pepperoni" : "margarita")

// Variables compartidas
int pizzas[2] = {0}; 
pthread_mutex_t mutex;        
pthread_cond_t faltaPizza[2];       
pthread_cond_t pizzaDisponible[2]; 

void conseguirPizza(int tipoPizza) {
    pthread_mutex_lock(&mutex);

    while (pizzas[tipoPizza] == 0) {
        printf("Cliente: No hay pizzas de %s. Esperando reposición.\n", PIZZASTR(tipoPizza));
        pthread_cond_signal(&faltaPizza[tipoPizza]); // Notifica al camarero, tmbien se puede usar broadcast ya qe solo tenemos 1 camarero
        pthread_cond_wait(&pizzaDisponible[tipoPizza], &mutex); // Espera reposición
    }

    pizzas[tipoPizza]--; // Toma una pizza
    printf("Cliente: Tomó una pizza de %s. Quedan: %d\n", PIZZASTR(tipoPizza), pizzas[tipoPizza]);

    pthread_mutex_unlock(&mutex);
}

void servirPizzas() {
    pthread_mutex_lock(&mutex);

    for (int tipoPizza = 0; tipoPizza < 2; tipoPizza++) {
        while (pizzas[tipoPizza] > 0) {
            pthread_cond_wait(&faltaPizza[tipoPizza], &mutex);
        }

    
        printf("Camarero: Reponiendo pizzas de %s.\n", PIZZASTR(tipoPizza));
        pizzas[tipoPizza] += N;
        pthread_cond_broadcast(&pizzaDisponible[tipoPizza]); // Notificar clientes
    }

    pthread_mutex_unlock(&mutex);
}

void *Cliente(void *arg) {
    int tipoPizza = *(int *)arg;
    free(arg);

    while (1) { 
        conseguirPizza(tipoPizza);
        printf("Cliente: Está comiendo pizza de %s.\n", PIZZASTR(tipoPizza));
        sleep((rand() % 3) + 1); 
    }
    return NULL;
}

void *Camarero(void *arg) {
    while (1) { 
        servirPizzas();
    }
    return NULL;
}

int main() {
    pthread_t thCliente[10];
    pthread_t thCamarero;

    pthread_mutex_init(&mutex, NULL);
    for (int i = 0; i < 2; i++) {
        pthread_cond_init(&faltaPizza[i], NULL);
        pthread_cond_init(&pizzaDisponible[i], NULL);
    }

    // Hilo camarero
    pthread_create(&thCamarero, NULL, Camarero, NULL);

    // Hilos clientes
    for (int i = 0; i < 10; i++) {
        int *tipo = malloc(sizeof(int));
        *tipo = rand() % 2; // Tipo de pizza preferido
        pthread_create(&thCliente[i], NULL, Cliente, tipo);
    }

    // Unir hilos de clientes
    for (int i = 0; i < 10; i++) {
        pthread_join(thCliente[i], NULL);
    }

    pthread_cancel(thCamarero); // Forzar detención del camarero
    pthread_join(thCamarero, NULL);

    pthread_mutex_destroy(&mutex);

    //pepperonis
    pthread_cond_destroy(&faltaPizza[0]);
    pthread_cond_destroy(&pizzaDisponible[0]);

    //margaritas
    pthread_cond_destroy(&faltaPizza[0]);
    pthread_cond_destroy(&pizzaDisponible[0]);

    return 0;
}
