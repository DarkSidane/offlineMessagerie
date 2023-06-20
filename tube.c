#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

void *th1(void *i);
void *th2(void *i);

int main() {
    pthread_t thread1;
    int *tube = (int *)malloc(sizeof(int) * 2);

    pipe(tube);

    printf("pipe est créé\n");

    if (pthread_create(&thread1, NULL, th1, (void *)tube) == -1) {
        fprintf(stderr, "Erreur creation pthread\n");
        exit(2);
    }

    pthread_join(thread1, NULL);
    printf("thread 1 est terminé\n");

    return 0;
}

void *th1(void *i) {
    int *tube = (int *)i;
    char buf[100];
    int len;
    pthread_t thread2;

    printf("thread 1\n");

    //close(tube[0]);
    strcpy(buf, "bonjour du fils");
    len = strlen(buf);
    write(tube[1], &len, sizeof(int));
    write(tube[1], buf, len);
    //close(tube[1]);

    if (pthread_create(&thread2, NULL, th2, (void *)tube) == -1) {
        fprintf(stderr, "Erreur creation pthread\n");
        exit(2);
    }

    pthread_join(thread2, NULL);
    printf("thread 2 est terminé\n");

    pthread_exit(0);
}


void *th2(void *i) {
    int *tube = (int *)i;
    char buf[100];
    int len;

    printf("thread 2\n");

    //close(tube[1]);
    read(tube[0], &len, sizeof(int));
    read(tube[0], buf, len);
    buf[len] = 0;
    printf("lu : %s\n", buf);
    //close(tube[0]);

    pthread_exit(0);
}
