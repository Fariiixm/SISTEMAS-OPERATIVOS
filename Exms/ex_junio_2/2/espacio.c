#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>

#define FOLDER_01 "Folder_01"
#define FOLDER_02 "Folder_02"

#define UMBRAL (300 * 1024) //(300 KB en bytes)

/* Forward declaration */
int get_size_dir(char *fname, size_t *blocks);

/* Gets in the blocks buffer the size of file fname using lstat. If fname is a
 * directory get_size_dir is called to add the size of its contents.
 */
int get_size(char *fname, size_t *blocks)
{
    struct stat file_stat;

    // Llamada a lstat para obtener información del archivo
    if (lstat(fname, &file_stat) == -1) {
        perror(fname);
        return -1;
    }

    // Sumar los bloques ocupados por el archivo actual
    *blocks += file_stat.st_blocks;

    // Si es un directorio, procesar su contenido
    if (S_ISDIR(file_stat.st_mode)) {
        return get_size_dir(fname, blocks);
    }

    return 0;
}

/* Gets the total number of blocks occupied by all the files in a directory. If
 * a contained file is a directory a recursive call to get_size_dir is
 * performed. Entries . and .. are conveniently ignored.
 */
int get_size_dir(char *dname, size_t *blocks)
{
    DIR *dir;
    struct dirent *entry;
    char path[PATH_MAX];

    // Abrir el directorio
    if ((dir = opendir(dname)) == NULL) {
        perror(dname);
        return -1;
    }

    // Leer todas las entradas del directorio
    while ((entry = readdir(dir)) != NULL) {
        // Ignorar las entradas "." y ".."
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        // Construir la ruta completa del archivo/directorio
        snprintf(path, sizeof(path), "%s/%s", dname, entry->d_name);


        /***************************************************************************************************************************** */
        /***************************************************************************************************************************** */
        /***************************************************************************************************************************** */

        // Verificamos si es una imagen .png
        size_t len = strlen(entry->d_name);
        if (len > 4 && strcmp(entry->d_name + len - 4, ".png") == 0) {
            struct stat sb;
            char dest_path[PATH_MAX];
            if (stat(path, &sb) == -1) { // Información de la imagen
                perror("Error al obtener la información del archivo");
                continue;
            }

            if (sb.st_size < UMBRAL) {
                // Imagen menor que el umbral, la movemos a Folder_02
                snprintf(dest_path, sizeof(dest_path), "%s/%s", FOLDER_02, entry->d_name);
                if (link(path, dest_path) != -1) 
                    printf("Enlace creado: %s -> %s\n", path, dest_path);
        
            } else {
                // Imagen mayor o igual al umbral, la movemos a Folder_01
                snprintf(dest_path, sizeof(dest_path), "%s/%s", FOLDER_01, entry->d_name);
                if (link(path, dest_path) != -1) 
                    printf("Enlace creado: %s -> %s\n", path, dest_path);
            
            }
        }

        /***************************************************************************************************************************** */
        /***************************************************************************************************************************** */
        /***************************************************************************************************************************** */

        // Recursión para procesar cada entrada
        if (get_size(path, blocks) == -1) {
            closedir(dir);
            return -1;
        }
    }

    closedir(dir);
    return 0;
}

/* Processes all the files in the command line calling get_size on them to
 * obtain the number of 512 B blocks they occupy and prints the total size in
 * kilobytes on the standard output
 */
int main(int argc, char *argv[])
{
    if (argc < 2) {
        fprintf(stderr, "Uso: %s <archivo/directorio> [<archivo/directorio> ...]\n", argv[0]);
        return EXIT_FAILURE;
    }

    for (int i = 1; i < argc; i++) {
        size_t blocks = 0;

        // Calcular el tamaño del archivo/directorio
        if (get_size(argv[i], &blocks) == -1) {
            fprintf(stderr, "Error procesando %s\n", argv[i]);
            continue;
        }

        // Convertir bloques de 512 bytes a kilobytes
        printf("%luK %s\n", (blocks + 1) / 2, argv[i]);
    }

    return 0;
}
