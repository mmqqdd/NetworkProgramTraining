#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
sem_t sem1, sem2;
int num;
void* read(void*);
void* calc(void*);
int main() {
    sem_init(&sem1, 0, 1);
    sem_init(&sem2, 0, 0);

    pthread_t tr_id1, tr_id2;
    pthread_create(&tr_id1, NULL, read, NULL);
    pthread_create(&tr_id2, NULL, calc, NULL);

    pthread_join(tr_id1, NULL);
    pthread_join(tr_id2, NULL);

    sem_destroy(&sem1);
    sem_destroy(&sem2);
}

void* read(void*) {
    int i;
    for (i = 0; i < 5; i++) {
        sem_wait(&sem1);
		printf("input a number \n");
        scanf("%d", &num);
        sem_post(&sem2);
    }
    return NULL;
}

void* calc(void*) {
    int i, ans = 0;
    for (i = 0; i < 5; i++) {
        sem_wait(&sem2);
        ans += num;
        sem_post(&sem1);
		printf("after %d times, ans is %d\n",i+1,ans);
    }
    printf("ans is %d", ans);
    return NULL;
}
