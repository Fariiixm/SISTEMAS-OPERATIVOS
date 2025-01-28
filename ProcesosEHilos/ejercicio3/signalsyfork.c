#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

/* Variable global para el PID del hijo */
pid_t child;

/* Manejador para la señal SIGALRM */
void handler(int signo) {
    if (child > 0) {
        printf("Alarma: enviando SIGKILL al hijo con PID %d...\n", child);

        kill(child, SIGKILL); // Enviar SIGKILL al hijo
    }
}

int main(int argc, char **argv) {
    if (argc < 2) {
        printf("Uso: %s <comando> [argumentos]\n", argv[0]);
        exit(EXIT_FAILURE);
    }


    /* Crear el proceso hijo */
    child = fork();

    if (child == 0) {
        /* Código del proceso hijo: ejecutar el comando */
        execvp(argv[1], &argv[1]);
        perror("Error al ejecutar el comando en el hijo"); // Mostrar error si execvp falla
        exit(EXIT_FAILURE);
    } else {
        /* Código del proceso padre */
        struct sigaction sa;
        sa.sa_handler = handler;
        sa.sa_flags = 0;
        sigemptyset(&sa.sa_mask);

		/* Configurar manejador de SIGALRM */
        if (sigaction(SIGALRM, &sa, NULL) == -1) {
            perror("Error en sigaction");
            exit(EXIT_FAILURE);
        }

		/* Ignorar la señal SIGINT */
    	signal(SIGINT, SIG_IGN);

        

        /* Configurar alarma */
        alarm(5);
        
		wait(NULL);

	}

	

    return 0;
}


//./signalsyfork sleep 10
//./signalsyfork ls -l
