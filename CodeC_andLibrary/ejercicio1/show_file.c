#include <stdio.h>
#include <stdlib.h>
#include <err.h>

#define BUFFER_SIZE 1024  // Tamaño del buffer para fread

int main(int argc, char* argv[]) {
    FILE* file = NULL;
    size_t bytesRead;
    unsigned char buffer[BUFFER_SIZE];

    if (argc != 2) {
        fprintf(stderr, "Usage: %s <file_name>\n", argv[0]);
        exit(1);
    }

    /* Open file */
    if ((file = fopen(argv[1], "r")) == NULL)
        err(2, "The input file %s could not be opened", argv[1]);

    /* Read file using fread */
    while ((bytesRead = fread(buffer, 1, BUFFER_SIZE, file)) > 0) {
        /* Write the buffer to stdout */
        if (fwrite(buffer, 1, bytesRead, stdout) != bytesRead) {
            fclose(file);
            err(3, "fwrite() failed!!");
        }
    }

    /* Check for errors in fread */
    if (ferror(file)) {
        fclose(file);
        err(4, "Error reading file");
    }

    fclose(file);
    return 0;
}
