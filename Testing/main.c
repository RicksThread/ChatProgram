#include <pthread.h>
#include <stdlib.h>

void* test_thread()
{
    printf("asdasd\n");
    sleep(1);
    pthread_exit((void*)34);
}

int main()
{
    pthread_t tid;
    int sus;
    pthread_create(&tid, NULL, &test_thread, NULL);
    pthread_join(tid, &sus);
    printf("value: %d\n", sus);
    return 0;
}