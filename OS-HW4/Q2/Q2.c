#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

// semaphore declarations
sem_t sem1, sem2;

void* p1(void* arg) {

    sem_wait(&sem2); 
    printf("F\n");

    sem_post(&sem1);
    printf("E\n");
    printf("G\n");

    return NULL;
}

void* p2(void* arg) {

    
    printf("A\n");
    sem_post(&sem2); 

    sem_wait(&sem1); 
    printf("C\n");
    printf("B\n");

    printf("C\n");
    printf("B\n");
    return NULL;
}

int main() {
    
    // semaphore initializations
    sem_init(&sem2, 0, 0);
    sem_init(&sem1, 0, 0);


    // thread creations
    pthread_t thread1, thread2;

    if (pthread_create(&thread1, NULL, p1, NULL) != 0) {
        perror("Thread creation failed");
        return 1;
    }

    if (pthread_create(&thread2, NULL, p2, NULL) != 0) {
        perror("Thread creation failed");
        return 1;
    }

    // Wait for threads to finish
    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);

    // semaphore destroy
    if (sem_destroy(&sem1) != 0 || sem_destroy(&sem2) != 0) {
        perror("Semaphore destruction failed");
        return 1;
    }

    return 0;
}
