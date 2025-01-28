
//cuestion A
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

int main(void)
{
    int fd1, i;
    char buffer[6];

    // Abrir el archivo de salida
    fd1 = open("output.txt", O_CREAT | O_TRUNC | O_RDWR, S_IRUSR | S_IWUSR);
    if (fd1 == -1) {
        perror("Error abriendo el archivo");
        exit(1);
    }

    // Escribir los ceros iniciales
    write(fd1, "00000", 5);

    // Crear los procesos hijos
    for (i = 1; i < 10; i++) {
        if (fork() == 0) {
            // Proceso hijo
            sprintf(buffer, "%d", i * 11111);  // Escribir el número correspondiente
            write(fd1, buffer, 5);              // Escribir en el archivo
            close(fd1);                         // Cerrar el archivo
            exit(0);                            // Terminar el proceso hijo
        } else {
            // Proceso padre espera que el hijo termine antes de escribir
            wait(NULL);  // Espera que termine el hijo
        }
    }

    // Leer y mostrar el contenido del archivo
    lseek(fd1, 0, SEEK_SET);  // Reposicionar el puntero al inicio del archivo
    printf("File contents are:\n");
    char c;
    while (read(fd1, &c, 1) > 0)
        printf("%c", c);  // Imprimir cada caracter leído
    printf("\n");

    close(fd1);  // Cerrar el archivo
    return 0;
}

//for i in $(seq 10); do ./inicial ; done


//cuestion b

/*#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

int main(void)
{
    int fd1, i;
    char buffer[6];

    // Abrir el archivo de salida
    fd1 = open("output.txt", O_CREAT | O_TRUNC | O_RDWR, S_IRUSR | S_IWUSR);
    if (fd1 == -1) {
        perror("Error abriendo el archivo");
        exit(1);
    }

    // Escribir los ceros iniciales
    write(fd1, "00000", 5);

    // Crear los procesos hijos
    for (i = 1; i < 10; i++) {
        if (fork() == 0) {
            // Proceso hijo
            // Posicionar el puntero al final del archivo para escribir de forma secuencial
            lseek(fd1, 0, SEEK_END);
            // Escribir el número correspondiente
            sprintf(buffer, "%d", i * 11111);  // Crear el número
            write(fd1, buffer, 5);              // Escribir en el archivo
            close(fd1);                         // Cerrar el archivo
            exit(0);                            // Terminar el proceso hijo
        } else {
            // Proceso padre espera que el hijo termine antes de escribir
            wait(NULL);  // Espera que termine el hijo
            // Después de que el hijo termine, el padre también escribe su número
            
            if(i < 9){
                lseek(fd1, 0, SEEK_END);  // Posicionar el puntero al final del archivo
                write(fd1, "00000", 5);   // El padre escribe 5 ceros
            }
        }
    }

    // Leer y mostrar el contenido del archivo
    lseek(fd1, 0, SEEK_SET);  // Reposicionar el puntero al inicio del archivo
    printf("File contents are:\n");
    char c;
    while (read(fd1, &c, 1) > 0)
        printf("%c", c);  // Imprimir cada caracter leído
    printf("\n");

    close(fd1);  // Cerrar el archivo
    return 0;
}*/

//for i in $(seq 10); do ./inicial ; done
