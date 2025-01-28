#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

#define MAX_LENGTH 256

void copy(int fdo, int fdd) {

	char buffer[MAX_LENGTH];

	/*while(read(fdo, buffer, sizeof(buffer)) != 0){
	}
	write(fdd, buffer, sizeof(buffer));*/

	ssize_t bytesRead;

	while((bytesRead = read(fdo, buffer, sizeof(buffer))) > 0) {
        write(fdd, buffer, bytesRead);
    }

}

int main(int argc, char *argv[])
{

	if(argc != 3){
		printf("Usage: %s <file1> <file2>\n", argv[0]);
		exit(1);
	}
	int fdo;
	if((fdo = open(argv[1], O_RDONLY, S_IRWXU)) == -1){
		printf("Error opening file %s\n", argv[1]);
		exit(1);
	}

	int fdd;

	if((fdd = open(argv[2], O_CREAT | O_WRONLY | O_TRUNC, S_IRWXU)) == -1){
		printf("Error creating %s\n", argv[2]);
		exit(1);
	}

	copy(fdo, fdd);
	close(fdo);
	close(fdd);



	return 0;
}

//./copy prueba.txt out1.txt 
//./check_copy 
