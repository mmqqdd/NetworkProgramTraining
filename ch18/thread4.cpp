#include <pthread.h>
#include <stdio.h>

#define N 2
#define NUM_THREAD 2

void* thread_inc(void*);
void* thread_dec(void*);

long long num = 0;

int main() {
    pthread_t tr_id[NUM_THREAD];
    int i;
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
}

void* thread_inc(void* args) {
    int i = 0;
    for (i = 0; i < N; i++)
        num += i;
    return NULL;
}

void* thread_dec(void* args) {
    int i = 0;
    for (i = 0; i < N; i++)
        num -= i;
    return NULL;
}