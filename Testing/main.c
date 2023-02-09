#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>

#define EXITCMD "cmd|exit"

void* test_thread(void* args)
{
    char* message = malloc(20);
    printf("insert the message:");

    fgets(message, 20, stdin);

    pthread_exit(message);
}

int main()
{
    pthread_t tid;
    char* sus;
    pthread_create(&tid, NULL, &test_thread, NULL);
    pthread_join(tid, &sus);

    format_linestr(sus);

    printf("message received: %s\n", sus);
    if (strcmp(sus, EXITCMD) == 0)
    {
        printf("exit's message\n");
    }
    free(sus);
    return 0;
}