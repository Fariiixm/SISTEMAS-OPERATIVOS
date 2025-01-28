#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Función auxiliar para leer una cadena de caracteres terminada en '\0' del fichero
char *loadstr(FILE *fichero) {
  // Obtener la posición actual del puntero de posición del fichero
  long posicion_actual = ftell(fichero);

  // Leer caracter a caracter hasta encontrar el terminador '\0'
  char caracter;
  int longitud_cadena = 0;
  do {
    caracter = fgetc(fichero);
    if (caracter == EOF) {
      // Se ha llegado al final del fichero sin encontrar el terminador
      return NULL;
    }
    longitud_cadena++;
  } while (caracter != '\0');

  // Reubicar el puntero de posición del fichero al inicio de la cadena
  fseek(fichero, posicion_actual, SEEK_SET);

  // Reservar memoria para la cadena y copiarla del fichero
  char *cadena = malloc((longitud_cadena + 1) * sizeof(char));
  if (cadena == NULL) {
    return NULL;
  }
  fread(cadena, sizeof(char), longitud_cadena, fichero);
  cadena[longitud_cadena] = '\0'; // Agregar el terminador manualmente

  return cadena;
}

int main(int argc, char *argv[]) {
  // Comprobar si se ha proporcionado el nombre del fichero
  if (argc != 2) {
    fprintf(stderr, "Uso: %s <fichero_entrada>\n", argv[0]);
    return 1;
  }

  // Abrir el fichero en modo lectura binaria
  FILE *fichero = fopen(argv[1], "rb");
  if (fichero == NULL) {
    perror("Error al abrir el fichero");
    return 1;
  }

  // Leer las cadenas de caracteres del fichero y mostrarlas por pantalla
  char *cadena;
  while ((cadena = loadstr(fichero)) != NULL) {
    printf("%s\n", cadena);
    free(cadena);
  }

  // Comprobar si se ha producido un error de lectura
  if (ferror(fichero)) {
    perror("Error al leer del fichero");
    fclose(fichero);
    return 1;
  }

  // Cerrar el fichero
  fclose(fichero);

  return 0;
}