/*
aa
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/wait.h>

pid_t launch_command(char** argv){
    
    pid_t child = fork();

    if(child == 0){
        if(execvp(argv[0], argv) == -1){
            printf("Error en execvp\n");
            exit(EXIT_FAILURE);
        }
    }

    return child;
}



char **parse_command(const char *cmd, int* argc) {
    // Allocate space for the argv array (initially with space for 10 args)
    size_t argv_size = 10;
    const char *end;
    size_t arg_len; 
    int arg_count = 0;
    const char *start = cmd;
    char **argv = malloc(argv_size * sizeof(char *));

    if (argv == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    while (*start && isspace(*start)) start++; // Skip leading spaces

    while (*start) {
        // Reallocate more space if needed
        if (arg_count >= argv_size - 1) {  // Reserve space for the NULL at the end
            argv_size *= 2;
            argv = realloc(argv, argv_size * sizeof(char *));
            if (argv == NULL) {
                perror("realloc");
                exit(EXIT_FAILURE);
            }
        }

        // Find the start of the next argument
        end = start;
        while (*end && !isspace(*end)) end++;

        // Allocate space and copy the argument
        arg_len = end - start;
        argv[arg_count] = malloc(arg_len + 1);

        if (argv[arg_count] == NULL) {
            perror("malloc");
            exit(EXIT_FAILURE);
        }
        strncpy(argv[arg_count], start, arg_len);
        argv[arg_count][arg_len] = '\0';  // Null-terminate the argument
        arg_count++;

        // Move to the next argument, skipping spaces
        start = end;
        while (*start && isspace(*start)) start++;
    }

    argv[arg_count] = NULL; // Null-terminate the array

    (*argc)=arg_count; // Return argc

    return argv;
}


int main(int argc, char *argv[]) {
    char **cmd_argv;
    int cmd_argc;

    //if (argc != 2) {
      //  fprintf(stderr, "Usage: %s \"command\"\n", argv[0]);
        //return EXIT_FAILURE;
    //}
    int opt;

    while((opt = getopt(argc, argv, "x:s:b")) != -1){
        switch(opt){
            case 'x':
                cmd_argv = parse_command(optarg, &cmd_argc);
                pid_t pid = launch_command(cmd_argv);
                int status;
                waitpid(pid, &status, 0);
                printf("@@ Command terminated (pid: %d, status: %d)\n", pid, WEXITSTATUS(status));
                for(int i = 0; i < cmd_argc;i++)
                    free(cmd_argv[i]);
                free(cmd_argv);
            case 's':
                FILE *file = fopen(optarg, "r");
                if (!file) {
                    perror("fopen");
                    exit(EXIT_FAILURE);
                }
                char line[1024];
                int command_number = 0;
                while (fgets(line, sizeof(line), file)) {
                    line[strcspn(line, "\n")] = '\0'; // Remove newline
                    printf("@@ Running command #%d: %s\n", command_number, line);
                    cmd_argv = parse_command(line, &cmd_argc);
                    pid_t pid = launch_command(cmd_argv);
                    int status;
                    waitpid(pid, &status, 0);
                    printf("@@ Command #%d terminated (pid: %d, status: %d)\n", command_number++, pid, WEXITSTATUS(status));
                    for (int i = 0; i < cmd_argc; i++) free(cmd_argv[i]);
                    free(cmd_argv);
                }
                fclose(file);
                break;
            
            case 'b': 
                // opcional!!!
                break;
            
            default:
                fprintf(stderr, "Usage: %s [-x command] [-s file] [-b]\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    return EXIT_SUCCESS;
}*/
/*# Testing -x switch
$ ./run_commands -x ls
Makefile        run_commands    run_commands.c  run_commands.o  test1           test2

$ ./run_commands -x "echo hello"
Hello

# Testing -s switch
$ ./run_commands -s test1
@@ Running command #0: echo hello
hello
@@ Command #0 terminated (pid: 1439, status: 0)

@@ Running command #1: sleep 2
@@ Command #1 terminated (pid: 1440, status: 0)
@@ Running command #2: ls -l
total 88
-rw-r--r--@ 1 usuarioso  usuarioso   267 Oct 20 11:34 Makefile
-rwxr-xr-x  1 usuarioso  usuarioso  9960 Oct 20 11:58 run_commands
-rw-r--r--  1 usuarioso  usuarioso  4332 Oct 20 11:57 run_commands.c
-rw-r--r--  1 usuarioso  usuarioso  8984 Oct 20 11:58 run_commands.o
-rw-r--r--@ 1 usuarioso  usuarioso  31 Oct 20 11:34 test1
-rw-r--r--@ 1 usuarioso  usuarioso  41 Oct 20 11:46 test2
@@ Command #2 terminated (pid: 1443, status: 0)
@@ Running command #3: false
@@ Command #3 terminated (pid: 1444, status: 256)


# Testing -bs switch
$ ./run_commands -b -s test2 
@@ Running command #0: echo one
@@ Running command #1: sleep 6
@@ Running command #2: sleep 3
@@ Running command #3: echo two
one
two
@@ Running command #4: sleep 1@@ Command #3 terminated (pid: 1457, status: 0)
@@ Command #0 terminated (pid: 1454, status: 0)
@@ Command #4 terminated (pid: 1458, status: 0)
@@ Command #2 terminated (pid: 1456, status: 0)
@@ Command #1 terminated (pid: 1455, status: 0)*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/wait.h>
#include <getopt.h>
#include <unistd.h>  

#define BUFFER_SIZE 512
#define MAX_COMMANDS 20

pid_t launch_command(char** argv){
    /* To be completed */
    pid_t pid = fork();
    if (pid < 0){
        perror("fork");
        return -1;
    }
    if (pid == 0){
        execvp(argv[0], argv);
        perror("execvp");        // Si execvp falla, imprime el error
        exit(EXIT_FAILURE);  
    }
    return pid;
}



char **parse_command(const char *cmd, int* argc) {
    // Allocate space for the argv array (initially with space for 10 args)
    size_t argv_size = 10;
    const char *end;
    size_t arg_len; 
    int arg_count = 0;
    const char *start = cmd;
    char **argv = malloc(argv_size * sizeof(char *));

    if (argv == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    while (*start && isspace(*start)) start++; // Skip leading spaces

    while (*start) {
        // Reallocate more space if needed
        if (arg_count >= argv_size - 1) {  // Reserve space for the NULL at the end
            argv_size *= 2;
            argv = realloc(argv, argv_size * sizeof(char *));
            if (argv == NULL) {
                perror("realloc");
                exit(EXIT_FAILURE);
            }
        }

        // Find the start of the next argument
        end = start;
        while (*end && !isspace(*end)) end++;

        // Allocate space and copy the argument
        arg_len = end - start;
        argv[arg_count] = malloc(arg_len + 1);

        if (argv[arg_count] == NULL) {
            perror("malloc");
            exit(EXIT_FAILURE);
        }
        strncpy(argv[arg_count], start, arg_len);
        argv[arg_count][arg_len] = '\0';  // Null-terminate the argument
        arg_count++;

        // Move to the next argument, skipping spaces
        start = end;
        while (*start && isspace(*start)) start++;
    }

    argv[arg_count] = NULL; // Null-terminate the array

    (*argc)=arg_count; // Return argc

    return argv;
}


int main(int argc, char *argv[]) {
    char **cmd_argv;
    int cmd_argc;
    int i;
    int o;
    int fichero = 0;
    char* comando;
    char* ruta;
    int status;
    int flag_b = 0;
    pid_t pids[MAX_COMMANDS];

    while ((o = getopt(argc, argv, "x:s:b")) != -1){
		switch (o){
			case 'x':
				fichero = 0;
                comando = optarg;
				break;
            case 's':
                fichero = 1;
                ruta = optarg;
                break;
            case 'b':
                if(fichero == 0)
                     fprintf(stderr, "Error: La opción -b debe usarse junto con -s.\n");
                else
                    flag_b = 1;
                break;
		}

	}
    if(fichero == 0){
        cmd_argv=parse_command(comando,&cmd_argc);
        pid_t pid = launch_command(cmd_argv);
        if (pid == -1) {
            fprintf(stderr, "Failed to launch command.\n");
            return EXIT_FAILURE;
        }
        waitpid(pid, &status, 0);
    }
    else{
        FILE* file = fopen(ruta, "r");
        char *linea = malloc(BUFFER_SIZE);
        int cont = 0;
        while(fgets(linea, BUFFER_SIZE, file) != NULL){   //lee línea a línea el fichero
            printf("@@ Running command #%d: %s\n", cont, linea);
            cmd_argv=parse_command(linea,&cmd_argc);

            pid_t pid = launch_command(cmd_argv);
            if (pid == -1) {
                fprintf(stderr, "Failed to launch command.\n");
                return EXIT_FAILURE;
            }
            if(!flag_b){
                waitpid(pid, &status, 0);
                printf("@@ Command #%d terminated (pid: %d, status: %d)\n", cont, pid, status);
            }

            pids[cont] = pid;
            cont++;
        }
        if(flag_b){
            for(int i = 0; i < cont; i++){
                waitpid(pids[i], &status, 0);
                printf("@@ Command #%d terminated (pid: %d, status: %d)\n", i, pids[i], status);
            }
            
        }
        free(linea);
        fclose(file);
    }
    
    // Print parsed arguments
    // printf("argc: %d\n", cmd_argc);
    // for (i = 0; cmd_argv[i] != NULL; i++) {
    //     printf("argv[%d]: %s\n", i, cmd_argv[i]);
    //     free(cmd_argv[i]);  // Free individual argument
    // }

    free(cmd_argv);  // Free the cmd_argv array

    return EXIT_SUCCESS;
}