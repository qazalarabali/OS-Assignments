
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>

sem_t sem_p1, sem_p2, sem_p3;

void *p1(void *arg) {
    // F
    sem_wait(&sem_p2); // Wait for signal from p2 to print F
    printf("F\n");

    // E
    sem_post(&sem_p3); // Signal that E can be printed
    sem_wait(&sem_p1); // Wait for signal from p2 to print E
    printf("E\n");

    // G
    printf("G\n");

    pthread_exit(NULL);
}

void *p2(void *arg) {
    // A
    printf("A\n");
    sem_post(&sem_p2); // Signal that A is done

    sem_wait(&sem_p3); // Wait for signal from p1 that E can be printed

    // C
    printf("C\n");

    // B
    printf("B\n");

    sem_post(&sem_p1); // Signal that E is done

    pthread_exit(NULL);
}

int main() {
    pthread_t tid1, tid2;

    // Initialize semaphores
    sem_init(&sem_p1, 0, 0);
    sem_init(&sem_p2, 0, 0);
    sem_init(&sem_p3, 0, 0);

    // Create threads
    pthread_create(&tid1, NULL, p1, NULL);
    pthread_create(&tid2, NULL, p2, NULL);

    // Wait for threads to finish
    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);

    // Destroy semaphores
    sem_destroy(&sem_p1);
    sem_destroy(&sem_p2);
    sem_destroy(&sem_p3);

    return 0;
}
