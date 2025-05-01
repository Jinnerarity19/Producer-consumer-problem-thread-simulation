#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>

#define BUFFER_SIZE 5
#define MAX 6 

int buffer[BUFFER_SIZE];
int in = 0, out = 0;

sem_t empty;
sem_t full;
pthread_mutex_t mutex;

void* prod(void* arg) {
    int pid = *(int*)arg;
    printf("[producer %d] starting\n", pid);
    int i;
    for (i = 0; i < MAX; i++) {
        printf("[producer %d] Waiting\n", pid);
        sem_wait(&empty);

        printf("[producer %d] Waiting\n", pid);
        pthread_mutex_lock(&mutex);

        buffer[in] = i;
        printf("[producer %d] produced item %d at index %d\n", pid, i, in);
        in = (in + 1) % BUFFER_SIZE;

        pthread_mutex_unlock(&mutex);
        printf("[producer %d] release after \n", pid);
        sem_post(&full);

        sleep(1);
    }

    printf("[producer %d] finished.\n", pid);
    return NULL;
}

void* con(void* arg) {
    int cid = *(int*)arg;
    printf("[consumer %d] starting\n", cid);
    int i;
    for (i = 0; i < MAX / 2; i++) {  
        printf("[consumer %d] waiting for item\n", cid);
        sem_wait(&full);

        printf("[consumer %d] waiting for lock\n", cid);
        pthread_mutex_lock(&mutex);

        int item = buffer[out];
        printf("[consumer %d]  got item %d from index %d\n", cid, item, out);
        out = (out + 1) % BUFFER_SIZE;

        pthread_mutex_unlock(&mutex);
        printf("[consumer %d] released lock after getting\n", cid);
        sem_post(&empty);

        sleep(1); 
    }

    printf("[consumer %d] finished.\n", cid);
    return NULL;
}

int main() {
    pthread_t prod_thread, cons_thread1, cons_thread2;
    int prod_id = 1, cons_id1 = 2, cons_id2 = 3;

    
    sem_init(&empty, 0, BUFFER_SIZE);
    sem_init(&full, 0, 0);
    pthread_mutex_init(&mutex, NULL);

    
    pthread_create(&prod_thread, NULL, prod, &prod_id);
    sleep(1); 
    pthread_create(&cons_thread1, NULL, con, &cons_id1);
    pthread_create(&cons_thread2, NULL, con, &cons_id2);

    
    pthread_join(prod_thread, NULL);
    pthread_join(cons_thread1, NULL);
    pthread_join(cons_thread2, NULL);

    sem_destroy(&empty);
    sem_destroy(&full);
    pthread_mutex_destroy(&mutex);

    printf("program done.\n");
    return 0;
}

