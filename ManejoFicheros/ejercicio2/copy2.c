#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#define MAX_LENGTH 256

void copy(int fdo, int fdd)
{
	ssize_t byteLeido;
	char buffer[MAX_LENGTH];
	while( (byteLeido = read(fdo, buffer, sizeof(buffer))) > 0){
		write(fdd, buffer, byteLeido);
	}
}

void copy_regular(char *orig, char *dest)
{
	int file1, file2;
		if( (file1 = open(orig, O_RDWR, S_IRWXU)) == -1){
			printf("Eror al oopen\n");
			exit(EXIT_FAILURE);
		}
	
	if( (file2 = open(dest, O_RDWR | O_CREAT | O_TRUNC, S_IRWXU)) == -1){
			printf("Error al oopen\n");
			exit(EXIT_FAILURE);
		}

	copy(file1, file2);
	close(file1);
	close(file2);

}

void copy_link(char *orig, char *dest)
{
	struct stat sb;

	lstat(orig, &sb);
	char *m;

	m = malloc( sb.st_size + 1);

	readlink(orig, m, sb.st_size + 1);
	m[sb.st_size]='\0';
	
	symlink(m, dest);
	
}

int main(int argc, char *argv[])
{

	if (argc < 2){
		printf("Usage: %s <mylink> <mylinkcopy>\n", argv[0]);
		exit(EXIT_FAILURE);	
	}

	struct stat sb;

	if(lstat(argv[1], &sb)){
		printf("Error en lstat\n");
		exit(EXIT_FAILURE);
	}

	

	switch(sb.st_mode & S_IFMT){

		case S_IFREG://regular
			copy_regular(argv[1], argv[2]);
		break;

		case S_IFLNK: //simbolico
			copy_link(argv[1], argv[2]);
		break;

		default:
			printf("Error no fichero regular, no fichero simbolico\n");
			exit(EXIT_FAILURE);
		break;
	}


	return 0;
}


/*$ ln -s ../ejercicio1/Makefile mylink
**$ ls -l
	...
	lrwxrwxrwx 1 christian christian   22 Jul 14 13:23 mylink -> ../ejercicio1/Makefile
	...
	$ ../ejercicio1/copy mylink mylinkcopy

	$ cp ../ejercicio1/copy.c copy2.c
*/