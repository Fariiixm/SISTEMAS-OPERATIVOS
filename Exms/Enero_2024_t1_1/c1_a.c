#include <stdio.h>
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

int main() {
    char command[] = "ls -l -t"; // Cadena de prueba
    int num_tokens;

    // Primera llamada para contar subcadenas
    num_tokens = setargs(command, NULL);
    printf("Número de subcadenas: %d\n", num_tokens);

    // Reservar espacio para argv
    char *argv[num_tokens + 1];

    // Segunda llamada para llenar argv
    setargs(command, argv);

    // Imprimir los valores en argv
    for (int i = 0; i < num_tokens; i++) {
        printf("argv[%d] = %s\n", i, argv[i]);
    }

    return 0;
}

