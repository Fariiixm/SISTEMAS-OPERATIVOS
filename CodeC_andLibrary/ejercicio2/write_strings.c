#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char* argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Uso: %s <nombre_archivo> string1 string2 ... stringN\n", argv[0]);
        return 1;
    }

    FILE* file = fopen(argv[1], "wb");
    if (file == NULL) {
        perror("Error al abrir el archivo");
        return 1;
    }

    for (int i = 2; i < argc; i++) {
        int len = strlen(argv[i]);
        // Escribir la cadena seguida del carácter nulo
        if (fwrite(argv[i], 1, len, file) != len || fwrite("\0", 1, 1, file) != 1) {
            perror("Error escribiendo en el archivo");
            fclose(file);
            return 1;
        }
    }

    fclose(file);
    return 0;
}