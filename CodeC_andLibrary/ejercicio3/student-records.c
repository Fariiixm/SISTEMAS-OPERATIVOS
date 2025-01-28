#include <stdio.h>
#include <unistd.h> 	// for getopt() 
#include <stdlib.h> 	// for EXIT_SUCCESS, EXIT_FAILURE
#include <string.h>
#include "defs.h"

// Supongamos que las lineas del archivo no tienen mas de 100 caracteres
#define MAXLEN_LINE_FILE 100

// Abre un archivo y muestra su contenido 
int print_text_file(char *path){
	// Part A
	FILE *file = fopen(path, "r");	// Abre el archivo ubicado en path en modo lectura (r) y lo apunta con *file
	if (file == NULL) {				// Si el puntero no apunta a nada:
        fprintf(stderr, "Error: No se pudo abrir el archivo %s\n", path);
        return EXIT_FAILURE;
    }

	char line[MAXLEN_LINE_FILE];	// Para almacenar cada linea leida
    int num_estudiantes;

	// Leer la 1ª linea (num_estudiantes)
    if (fgets(line, MAXLEN_LINE_FILE, file) == NULL) {		// Lee linea y la guarda en *line
        fprintf(stderr, "Error: No se pudo leer el número de estudiantes\n");
        fclose(file);
        return EXIT_FAILURE;
    }
    sscanf(line, "%d", &num_estudiantes);	// Extrae el numero de estudiantes y lo guarda en num_estudiantes
	
	// Lee cada estudiante
    for (int i = 0; i < num_estudiantes; i++) {	
        if (fgets(line, MAXLEN_LINE_FILE, file) == NULL) {	// Lee linea y la guarda en *line
            fprintf(stderr, "Error: Archivo terminado inesperadamente\n");
            break;
        }

		// Extrae los campos de cada estudiante
        student_t student;
        char *token;
        char *lineptr = line;	// Puntero que apunta a la cadena donde se guarda las lineas leidas

		// strsep para dividir los campos separados por ":"
        token = strsep(&lineptr, ":");		// *token apunta al 1er campo
        student.student_id = atoi(token);	// atoi convierte el 1er token en un entero

        token = strsep(&lineptr, ":");				// token apunta al 2º campo (hasta los ":")
        strncpy(student.NIF, token, MAX_CHARS_NIF);	// Copia el 2o campo en NIF, se asegura de no exceder el MAX

        student.first_name = strsep(&lineptr, ":");	// Guarda el 3er campo (hasta los ":")
        student.last_name = strsep(&lineptr, "\n"); // Guarda el 4º campo (hasta el salto de linea)

        // Imprimir el registro del estudiante i
        printf("[Entry #%d]\n", i);
        printf("\tstudent_id=%d\n", student.student_id);
        printf("\tNIF=%s\n", student.NIF);
        printf("\tfirst_name=%s\n", student.first_name);
        printf("\tlast_name=%s\n", student.last_name);
    }
    fclose(file);
	return 0;	// return EXIT_SUCCESS;
}

// Leer el archivo, crear una representacion binaria y 
// escribirla en el archivo binario especificado
int write_binary_file(char *input_file, char *output_file){
	// Part B
	FILE *infile, *outfile;				// Punteros a los archivos de entrada y salida
	char line[MAXLEN_LINE_FILE];		// Para almacenar las lineas leidas
	int num_estudiantes;

	// Abrir el archivo de entrada en modo lectura
	infile = fopen(input_file, "r");	// *infile apunta al archivo de entrada
	if(infile == NULL){					// Si el puntero no apunta a nada:
		fprintf(stderr, "Error: No se pudo abrir el archivo de entrada %s\n", input_file);
		return EXIT_FAILURE;
	}

	// Leer la 1ª linea (num_estudiantes)
	if(fgets(line, sizeof(line), infile) == NULL){	// Si la linea esta vacia:
		fprintf(stderr, "Error: No se puede leer el numero de estudiantes\n");
		fclose(infile);
		return EXIT_FAILURE;
	}
	// Si la linea NO esta vacia -> Guardamos el numero de estudiantes
	sscanf(line, "%d", &num_estudiantes);	// sscanf covierte la linea leida en un entero y la guarda en num_estudiantes

	// Abrir el archivo de salida en modo binario
	outfile = fopen(output_file, "wb");	// *outfile apunta al archivo de salida
	if(outfile == NULL){				// Si el puntero no apunta a nada:
		fprintf(stderr, "Error: No se pudo abrir el archivo de salida %s\n", output_file);
		fclose(infile);
		return EXIT_FAILURE;
	}

	// Escribir el numero de estudiantes al inicio del archivo binario
	fwrite(&num_estudiantes, sizeof(int), 1, outfile);

	student_t student;
	// Leer y escribir cada estudiante en formato binario
	while(fgets(line, sizeof(line), infile) != NULL){	// Mientras que la linea NO este vacia...
		// Parsear:
		// strtok para dividir la cadena en campos separados por ":"
		char *token = strtok(line, ":");	// *token apunta al 1er campo	
		if(token){		// Si el token apunta a algo (!= NULL):
			// ID
			student.student_id = atoi(token);	// atoi convierte char en int y lo guarda
			
			// NIF
			token = strtok(NULL, ":");					// token apunta al 2º campo (hasta los ":")
			strncpy(student.NIF, token, MAX_CHARS_NIF);	// Guarda el 2º campo
			student.NIF[MAX_CHARS_NIF] = '\0';			// Añade el terminador

			// NOMBRE
			token = strtok(NULL, ":");				// token apunta al 3er campo (hasta los ":")
			student.first_name = strdup(token);		// Guarda el 3er campo

			// APELLIDO
			token = strtok(NULL, ":\n");			// token apunta al 4º campo (hasta el salto de linea)
			student.last_name = strdup(token);		// Guarda el 4º campo

			// Escribir los datos del estudiante en el archivo binario
			fwrite(&student.student_id, sizeof(int), 1, outfile);
			fwrite(student.NIF, sizeof(char), MAX_CHARS_NIF + 1, outfile);
            fwrite(student.first_name, sizeof(char), strlen(student.first_name) + 1, outfile);
            fwrite(student.last_name, sizeof(char), strlen(student.last_name) + 1, outfile);
			
			// Liberar memoria de nombres
			free(student.first_name);
			free(student.last_name);
		}
	}
	fclose(infile);
	fclose(outfile);

	printf("%d student records written successfully to binary file %s\n", num_estudiantes, output_file);
	return 0;
}

// Mostrar contenido de un archivo binario
int print_binary_file(char *path){
	// Part C
	FILE *file = fopen(path, "rb");		// Abre el archivo en modo lectura binaria
	if (file == NULL) {
        fprintf(stderr, "Error: No se pudo abrir el archivo %s\n", path);
        return EXIT_FAILURE;
	}

	int num_estudiantes;
	// Leer el numero de estudiantes
	// fread va a leer 1 entero (int) (4 bytes)
	if(fread(&num_estudiantes, sizeof(int), 1, file) != 1){	
		// Si fread NO lee 1 entero (int) muestra error:
		fprintf(stderr, "Error: No se puede leer el numero de estudiantes\n");
		fclose(file);
		return EXIT_FAILURE;
	}
	
	// Leer datos de los estudiantes
	for (int i = 0; i < num_estudiantes; i++){
		student_t student;

		// ID -------------------------------------------------------------------------
		if(fread(&student.student_id, sizeof(int), 1, file) != 1){
			// Si fread NO lee 1 entero (int) muestra error:
			fprintf(stderr, "Error: No se pudo leer el ID\n");
            fclose(file);
            return EXIT_FAILURE;
		}

		// NIF ------------------------------------------------------------------------
		// MAX_CHARS_NIF + 1 para incluir \0
		if (fread(student.NIF, sizeof(char), MAX_CHARS_NIF + 1, file) != MAX_CHARS_NIF + 1) {
            // Si fread NO lee una cadena (char) de MAX_CHARS_NIF + 1 muestra error:
			fprintf(stderr, "Error: No se pudo leer el NIF\n");
            fclose(file);
            return EXIT_FAILURE;
        }

		// NOMBRE ---------------------------------------------------------------------
		int length = 0;
		long pos_ini = ftell(file);	// Guarda la posicion actual para poder regresar
		int ch;

		// Contar caracteres hasta encontrar el terminador '\0' o el fin (EOF)
        while ((ch = fgetc(file)) != EOF && ch != '\0') length++;

		if (ch == EOF) {	// Error porque tiene que terminar en \0
            fprintf(stderr, "Error: No se pudo leer el nombre\n");
            fclose(file);
            return EXIT_FAILURE;
        }

		// Volver a la posicion de inicio y leer el nombre
        fseek(file, pos_ini, SEEK_SET);		// fseek() mueve el puntero a la posicion inicial
        student.first_name = (char *)malloc((length + 1) * sizeof(char));	// malloc reserva memoria
        if (student.first_name == NULL) {
            fprintf(stderr, "Error: Memoria insuficiente para el nombre\n");
            fclose(file);
            return EXIT_FAILURE;
        }
        if (fread(student.first_name, sizeof(char), length + 1, file) != length + 1) {
            // Si fread NO lee una cadena (char) de length + 1 muestra error:
			fprintf(stderr, "Error: No se pudo leer el nombre\n");
            free(student.first_name);
            fclose(file);
            return EXIT_FAILURE;
        }

		// APELLIDO -------------------------------------------------------------------
		length = 0;
        pos_ini = ftell(file);	// Guarda la posicion actual para poder regresar
        
		// Contar caracteres hasta encontrar el terminador '\0' o el fin (EOF)
        while ((ch = fgetc(file)) != EOF && ch != '\0') length++;
        if (ch == EOF) {	// Error porque tiene que terminar en \0
            fprintf(stderr, "Error: No se pudo leer el apellido completo\n");
            free(student.first_name);
            fclose(file);
            return EXIT_FAILURE;
        }

		// Volver a la posicion de inicio y leer el apellido
		fseek(file, pos_ini, SEEK_SET);		// fseek() mueve el puntero a la posicion inicial
        student.last_name = (char *)malloc((length + 1) * sizeof(char));
        if (student.last_name == NULL) {
            fprintf(stderr, "Error: Memoria insuficiente para el apellido\n");
            free(student.first_name);
            fclose(file);
            return EXIT_FAILURE;
        }
        if (fread(student.last_name, sizeof(char), length + 1, file) != length + 1) {
			// Si fread NO lee una cadena (char) de length + 1 muestra error:
            fprintf(stderr, "Error: No se pudo leer el apellido\n");
            free(student.first_name);
            free(student.last_name);
            fclose(file);
            return EXIT_FAILURE;
        }

		// Imprimir los datos del estudiante
        printf("[Entry #%d]\n", i);
        printf("\tstudent_id=%d\n", student.student_id);
        printf("\tNIF=%s\n", student.NIF);
        printf("\tfirst_name=%s\n", student.first_name);
        printf("\tlast_name=%s\n", student.last_name);

        // Liberar memoria asignada 
        free(student.first_name);
        free(student.last_name);
	}
	fclose(file);
	return 0;
}

int main(int argc, char *argv[]){
	int ret_code, opt;
	struct options options;

	// Inicializa valores predeterminados for options
	options.input_file = NULL;
	options.output_file = NULL;
	options.action = NONE_ACT;
	ret_code = 0;

	// Parse command-line options
	while ((opt = getopt(argc, argv, "hi:po:b")) != -1){
		switch (opt){
		case 'h':
			fprintf(stderr, "Usage: %s [ -h | -p | -i file | -o <output_file> | -b ]\n", argv[0]);
			exit(EXIT_SUCCESS);
		case 'i':
			options.input_file = optarg;		// Espera un archivo y lo asigna a input_file
			break;

		// To be completed ...
		case 'p':
            options.action = PRINT_TEXT_ACT;	// Imprime de un archivo normal
            break;
		case 'o':								// Escribe en archivo binario
            options.output_file = optarg;		// Espera un archivo y lo asigna a output_file
			options.action = WRITE_BINARY_ACT;	// Establece la accion WRITE_BINARY_ACT
			break;
		case 'b':
            options.action = PRINT_BINARY_ACT;	// Imprime de un archivo binario
            break;
		default:
			exit(EXIT_FAILURE);
		}
	}

	// Verificacion de los argumentos requeridos
	if (options.input_file == NULL){
		fprintf(stderr, "Must specify one record file as an argument of -i\n");
		exit(EXIT_FAILURE);
	}

	// Ejecutar la opcion especificada
	switch (options.action){
	case NONE_ACT:
		fprintf(stderr, "Must indicate one of the following options: -p, -o, -b \n");
		ret_code = EXIT_FAILURE;
		break;
	case PRINT_TEXT_ACT:
		// Part A
		ret_code = print_text_file(options.input_file);		// Funcion a completar
		break;
	case WRITE_BINARY_ACT:
		// Part B
		ret_code = write_binary_file(options.input_file, options.output_file);	// Funcion a completar
		break;
	case PRINT_BINARY_ACT:
		// Part C
		ret_code = print_binary_file(options.input_file);	// Funcion a completar
		break;
	default:
		break;
	}
	exit(ret_code);
}
