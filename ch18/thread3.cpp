#include<stdio.h>
#include<pthread.h>
#include<unistd.h>

int sum = 0;
void * thread_sum(void * arg);
int main(){
    pthread_t id_t1, id_t2;
    int rang1[] = {1,5}, rang2[] = {6,10};
    pthread_create(&id_t1, NULL, thread_sum, (void*)&rang1);
    pthread_create(&id_t2, NULL, thread_sum, (void*)&rang2);
    pthread_join(id_t1, NULL);
    pthread_join(id_t2, NULL);
    printf("sum is %d\n",sum);
}

void * thread_sum(void* arg){
    int *rang = (int *)arg;
    int i;
    for(i = rang[0];i<=rang[1];i++)
        sum+=i;
    return NULL;
}