#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>
#include <sys/stat.h>

#define BUFFER_SIZE 256

void *pipe_reader(void *arg);
void *pipe_writer(void *arg);

int main(int argc, char *argv[])
{
    printf("Bienvenue %s!\n", argv[3]);
    pthread_t input_thread, output_thread;
    return 0;
}

void *pipe_reader(void *arg)
{
    char *fifo = (char *)arg;
    int input_pipe = open(fifo, O_RDONLY);

    if (input_pipe == -1) {
        perror("Erreur lors de l'ouverture des FIFO");
        exit(EXIT_FAILURE);
    }

    char buffer[BUFFER_SIZE];
    while (1)
    {
        read(input_pipe, buffer, BUFFER_SIZE);
        printf("Message reçu : %s\n", buffer);
    }

    pthread_exit(NULL);
}

void *pipe_writer(void *arg)
{
    char *fifo = (char *)arg;
    int output_pipe = open(fifo, O_WRONLY);

    if (output_pipe == -1)
    {
        perror("Erreur lors de l'ouverture des FIFO");
        exit(EXIT_FAILURE);
    }

    char buffer[BUFFER_SIZE];
    while (1)
    {

        while (fgets(buffer, BUFFER_SIZE, stdin) != NULL)
        {
            buffer[strcspn(buffer, "\n")] = 0;
            break;
        }
        fflush(stdin);
        write(output_pipe, buffer, strlen(buffer));
    }

    pthread_exit(NULL);
}
