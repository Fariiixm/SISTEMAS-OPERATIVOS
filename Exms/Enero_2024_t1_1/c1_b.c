#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <ctype.h>

// Declaración de la función setargs
int setargs(char *args, char **argv) {
    int cont = 0;

    // Ignorar los espacios iniciales
    while (*args && isspace(*args)) {
        args++;
    }

    if (argv == NULL) {
        while (*args) {
            if (!isspace(*args)) { // Si no hay espacio, se ha detectado un carácter
                cont++; // Incrementamos cont
                while (*args && !isspace(*args)) {
                    args++;
                }
            } else {
                args++;
            }
        }
        return cont;
    }else{

        while(*args){
            if(!isspace(*args)){
                
                argv[cont] = args;
                cont++;

                while(*args && !isspace(*args)){
                    args++;
                }
                
                if(*args){
                    *args = '\0';
                    args++;
                }
            }
        }

        if(argv != NULL){
            argv[cont] = NULL;
        }
    
    }
    return cont; // Solo para completar la función; no es necesario en este caso
}

int main(int argc, char *argv[]) {
    // Verificar que se pase el nombre del archivo como parámetro
    if (argc != 2) {
        fprintf(stderr, "Uso: %s <nombre_del_archivo>\n", argv[0]);
        return 1;
    }

    FILE *file = fopen(argv[1], "r");
    if (file == NULL) {
        perror("Error al abrir el archivo");
        return 1;
    }

    char line[1024]; // Buffer para leer líneas del archivo
    while (fgets(line, sizeof(line), file) != NULL) {
        // Eliminar el salto de línea final
        line[strcspn(line, "\n")] = '\0';

        // Primera llamada a setargs con argv == NULL
        int num_tokens = setargs(line, NULL);
        if (num_tokens == 0) {
            continue; // Si no hay comandos, pasar a la siguiente línea
        }

        // Reservar espacio para el array argv
        char *cargv[num_tokens + 1];

        // Segunda llamada a setargs con argv != NULL
        setargs(line, cargv);

        // Crear un proceso hijo para ejecutar el comando
        pid_t pid = fork();
        if (pid < 0) {
            perror("Error al crear el proceso hijo");
            fclose(file);
            return 1;
        }

        if (pid == 0) {
            // Proceso hijo: ejecutar el comando
            execvp(cargv[0], cargv);
            // Si execvp falla:
            perror("Error al ejecutar el comando");
            exit(1);
        } else {
            // Proceso padre: esperar a que el hijo termine
            int status;
            waitpid(pid, &status, 0);
        }
    }

    fclose(file);
    return 0;
}