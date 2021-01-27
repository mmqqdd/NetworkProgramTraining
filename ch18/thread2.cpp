#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void* thread_main(void*);
int main() {
    pthread_t t_id;
    int thread_args = 5;
    void* thr_ret;
    pthread_create(&t_id, NULL, thread_main, (void*)&thread_args);
    pthread_join(t_id, &thr_ret);
    printf("Thread return message: %s", (char*)thr_ret);
    free(thr_ret);
}

void* thread_main(void* args) {
    int cnt = *((int*)args);
    char* buf = (char*)malloc(50 * sizeof(char));
    strcpy(buf, "hello I am thread_main!");
    int i;
    for (i = 0; i < cnt; i++) {
        sleep(0.5);
        printf("thread is running!\n");
    }
    return (void*)buf;
}