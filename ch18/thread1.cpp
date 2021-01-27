#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

void* thread_main(void*);
int main() {
    pthread_t t_id;
    int thread_param = 5;
    if (pthread_create(&t_id, NULL, thread_main, (void*)&thread_param)) {
        puts("pthread_create() error");
        return -1;
    }
    sleep(10);
    puts("end of main\n");
    return 0;
}

void* thread_main(void* arg) {
    int i;
    int cnt = *((int*)arg);
    for (i = 0; i < cnt; i++) {
        sleep(1);
        printf("thread_main %d running\n", i);
    }
    return NULL;
}
