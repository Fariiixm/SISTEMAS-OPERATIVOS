#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

#define MAX_LENGTH 256

int main(int argc, char *argv[])
{
    if(argc < 2){
        printf("Usages....\n");
        exit(EXIT_FAILURE);
    }

	int N, opt, e_flag;
    off_t ls;

    int fd;
    
           
    while ((opt = getopt(argc, argv, "n:e")) != -1) {
        switch (opt) {
        case 'n':
            N = atoi(optarg);// char y hacer atoi??
            if (N < 0) {
                fprintf(stderr, "El valor de N debe ser positivo.\n");
                exit(EXIT_FAILURE);
            }
            break;
        case 'e':
            /*if((ls = lseek(fd, - N, SEEK_END)) == -1){
                printf("error lseek e\n");
                exit(EXIT_FAILURE);
            }*/
            e_flag = 1;
            break;
        default: 
            fprintf(stderr, "Uso: %s [-n N] [-e] <archivo>\n", argv[0]);
           /* exit(EXIT_FAILURE);
            if((ls = lseek(fd, N, SEEK_SET)) == -1){
                printf("error lseek e\n");
                exit(EXIT_FAILURE);
            }
*/
        }
    }

    if (optind >= argc) {
        fprintf(stderr, "Expected argument after options\n");
        exit(EXIT_FAILURE);
    }

    if((fd = open(argv[optind], O_RDONLY, S_IRWXU )) < 0){
        printf("Error en open\n");
        exit(EXIT_FAILURE);
    }

    if (e_flag) {
        
        ls = lseek(fd, -N, SEEK_END);
        if (ls == -1) {
            printf("Error al posicionar el marcador (-e)");
            close(fd);
            exit(EXIT_FAILURE);
        }
    } else {
        ls = lseek(fd, N, SEEK_SET);
        if (ls == -1) {
            printf("Error al posicionar el marcador");
            close(fd);
            exit(EXIT_FAILURE);
        }
    }
    

    char buffer[MAX_LENGTH];
    ssize_t bytesLeidos;
    while((bytesLeidos = read (fd, buffer, sizeof(buffer))) > 0){
        write(STDOUT_FILENO,buffer, bytesLeidos);
    }

    

    close(fd);

	return 0;
}
//./mostrar -n 5 prueba.txt -e
//./mostrar -n 5 prueba.txt 