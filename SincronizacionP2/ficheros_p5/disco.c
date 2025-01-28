#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#define CAPACITY 5
#define VIPSTR(vip) ((vip) ? "  vip  " : "not vip")

typedef struct{
	int id; 
	int isvip; // 0 no es vip, 1 es vip
} cliente_t;

int clientes_dentro = 0;
int vips_esperando = 0;
int turno_global = 0;
int turno_cliente = 0;
int turno_global_vip = 0;
int turno_cliente_vip = 0;
pthread_cond_t cond, cond_vip;
pthread_mutex_t mutex;

void enter_normal_client(int id)
{
	// mutex
	pthread_mutex_lock(&mutex);

	int mi_turno = turno_cliente;
	turno_cliente++;

	while(vips_esperando != 0 || mi_turno != turno_global || clientes_dentro >= CAPACITY){
		printf("Client %2d (not vip) waiting for entering in disco\n", id);	
		pthread_cond_wait(&cond, &mutex);
	}

	printf("Client %2d (not vip) entering in disco\n", id);

	clientes_dentro++;
	turno_global++;
	pthread_cond_broadcast(&cond);

	pthread_mutex_unlock(&mutex);
}

void enter_vip_client(int id)
{
	// mutex
	pthread_mutex_lock(&mutex);

	int mi_turno = turno_cliente_vip;
	turno_cliente_vip++;
	vips_esperando++;

	while(mi_turno != turno_global_vip || clientes_dentro >= CAPACITY){
		printf("Client %2d (vip) waiting for entering in disco\n", id);	
		pthread_cond_wait(&cond_vip, &mutex);
	}

	printf("Client %2d (vip) entering in disco\n", id);

	vips_esperando--;
	clientes_dentro++;
	turno_global_vip++;

	if (vips_esperando)
		pthread_cond_broadcast(&cond_vip);
	else	
		pthread_cond_broadcast(&cond);

	pthread_mutex_unlock(&mutex);
}

void dance(int id, int isvip)
{	
	printf("Client %2d (%s) dancing in disco\n", id, VIPSTR(isvip));
	sleep((rand() % 3) + 1);
}

void disco_exit(int id, int isvip)
{

	pthread_mutex_lock(&mutex);
	clientes_dentro--;

	if (vips_esperando)
		pthread_cond_broadcast(&cond_vip);
	else	
		pthread_cond_broadcast(&cond);
	

	printf("Client %2d (%s) is exiting the disco\n", id, VIPSTR(isvip));

	pthread_mutex_unlock(&mutex);

}

void *client(void *arg)
{
	int id = ((cliente_t*)arg)->id;
    int isvip = ((cliente_t*)arg)->isvip;
	 if ( isvip )
        enter_vip_client(id);
    else
        enter_normal_client(id);
    dance(id, isvip);
    disco_exit(id, isvip);

}

int main(int argc, char *argv[])
{

	if (argc!=2) {
        fprintf(stderr,"Usage: %s <file_name>\n",argv[0]);
        return 1;
    }

	FILE* file = fopen(argv[1], "r");

	if (file == NULL) {
        // Comprobar si el archivo se abrió correctamente.
        printf("No se pudo abrir el archivo.\n");
        return 1;
    }

	int n_clientes;

	// Leer el contenido del archivo usando fscanf
	if (fscanf(file, "%d", &n_clientes) != 1) {
		fprintf(stderr, "Error leyendo el número de clientes desde el archivo.\n");
		fclose(file);
    	return 1;
	}

	// Inicializar el mutex y las variables de condición
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond, NULL);
    pthread_cond_init(&cond_vip, NULL);

	pthread_t hilos[n_clientes];

	for(int i = 0; i < n_clientes; i++){

		cliente_t* cliente = (cliente_t*)malloc(sizeof(cliente_t));
		
		if (cliente == NULL) {
			perror("Error asignando memoria");
			exit(EXIT_FAILURE);
		}

		if (fscanf(file, "%d", &cliente->isvip) != 1) {
			fprintf(stderr, "Error leyendo los clientes desde el archivo.\n");
			fclose(file);
    		return 1;
		}
		cliente->id = i;
		if(pthread_create(&hilos[i], NULL, client, (void*)cliente) != 0){
			perror("Error creando hilo");
            free(cliente); // Liberar memoria en caso de error
            exit(EXIT_FAILURE);
		}
	}

	for (int i = 0; i < n_clientes; i++) {
        if (pthread_join(hilos[i], NULL) != 0) {
            perror("Error esperando hilo");
            exit(EXIT_FAILURE);
        }
    }

	fclose(file);

	return 0;
}  // <3
