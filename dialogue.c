#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>
#include <sys/stat.h>
#include <time.h>

#define BUFFER_SIZE 256 
#define ARG_SIZE 256

void *pipe_reader(void *arg);
void *pipe_writer(void *arg);

int main(int argc, char *argv[]) {

    printf("Bienvenue %s!\n", argv[3]);

    char arg[4][BUFFER_SIZE]; 

    pthread_t input_thread, output_thread;

    strcpy(arg[0], argv[3]);
    strcpy(arg[1], argv[1]);
    strcpy(arg[2], argv[2]);
    strcpy(arg[3], argv[4]);
    pthread_create(&output_thread, NULL, pipe_writer, (void *)arg);
    pthread_create(&input_thread, NULL, pipe_reader, (void *)arg);

    pthread_join(input_thread, NULL);
    pthread_join(output_thread, NULL);

    unlink(argv[1]);
    unlink(argv[2]);

    return 0;
}

void *pipe_reader(void *arg) {

    char (*array)[BUFFER_SIZE] = (char (*)[BUFFER_SIZE])arg;
    char *fifo = (char *)array[1];
    int input_pipe = open(fifo, O_RDONLY);

    if (input_pipe == -1) { perror("Erreur lors de l'ouverture des FIFO"); exit(EXIT_FAILURE); }

    char buffer[BUFFER_SIZE];
    while (1) {

        memset(buffer, 0, sizeof(buffer));
        read(input_pipe, buffer, BUFFER_SIZE);
        if(strcmp(buffer, "/quitter") == 0) {
            printf("L'interlocuteur a quitté la conversation\n");
            sleep(1);
            exit(EXIT_SUCCESS);
        }
        char messageComplet[BUFFER_SIZE+20] = "";
        char heure[BUFFER_SIZE+20] = "";
        time_t now = time(NULL);
        struct tm *lt = localtime(&now);
        strftime(heure, sizeof(heure), "(%H:%M) : ", lt);
        strcpy(messageComplet, (char *)array[3]);
        strcat(messageComplet, " ");
        strcat(messageComplet, heure);
        strcat(messageComplet, buffer);

        printf("\n%s\n", messageComplet);
    }

    pthread_exit(NULL);
}

void *pipe_writer(void *arg) {

    char (*array)[BUFFER_SIZE] = (char (*)[BUFFER_SIZE])arg;
    char *fifo = (char *)array[2];
    int output_pipe = open(fifo, O_WRONLY);

    if (output_pipe == -1) { perror("Erreur lors de l'ouverture des FIFO"); pthread_exit(NULL); }

    char heure[BUFFER_SIZE+20] = "";
    time_t now = time(NULL);
    struct tm *lt = localtime(&now);
    strftime(heure, sizeof(heure), "(%H:%M) : ", lt);
    printf("%s %s", (char *)array[0], heure);

    char buffer[BUFFER_SIZE];

    while (1) {

	    while (fgets(buffer, BUFFER_SIZE, stdin) != NULL) {
		    buffer[strcspn(buffer, "\n")] = 0;
		    break;
	    }
	    fflush(stdin);

	    if(strcmp(buffer, "/quitter") == 0) {
            
		    printf("Vous avez quitté la conversation\n");
		    write(output_pipe, buffer, strlen(buffer));
		    sleep(1);
		    exit(EXIT_SUCCESS);
	    }

	    write(output_pipe, buffer, strlen(buffer));
        printf("%s %s", (char *)array[0], heure);
    }

    pthread_exit(NULL);
}
