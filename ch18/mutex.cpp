#include <pthread.h>
#include <stdio.h>

#define N 50000
#define NUM_THREAD 100

void* thread_inc(void*);
void* thread_dec(void*);

long long num = 0;
pthread_mutex_t mutex;

int main() {
    pthread_t tr_id[NUM_THREAD];
    int i;
    pthread_mutex_init(&mutex, NULL);
    for (i = 0; i < NUM_THREAD; i++) {
        if (i & 1)
            pthread_create(tr_id + i, NULL, &thread_inc, NULL);
        else
            pthread_create(tr_id + i, NULL, &thread_dec, NULL);
    }
    for (i = 0; i < NUM_THREAD; i++) {
        pthread_join(tr_id[i], NULL);
    }

    printf("ans is %lld\n", num);
    pthread_mutex_destroy(&mutex);
    return 0;
}

void* thread_inc(void* args) {
    int i = 0;
    for (i = 0; i < N; i++) {
        pthread_mutex_lock(&mutex);
        num += i;
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

void* thread_dec(void* args) {
    int i = 0;
    for (i = 0; i < N; i++) {
        pthread_mutex_lock(&mutex);
        num -= i;
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}