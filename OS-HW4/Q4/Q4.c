#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <stdlib.h>

#define NUM_RESOURCES 5
#define NUM_THREADS 10

typedef struct {
    int resources[NUM_RESOURCES];
    sem_t availableResources;
    pthread_mutex_t poolMutex;
} ResourceManager;

ResourceManager resourceManager;

void *threadFunction(void *threadId) {
    int id = *((int *)threadId);

    // Simulate work with a random duration
    int workDuration = rand() % 5 + 1;
    sleep(workDuration);

    // Acquire resource
    sem_wait(&resourceManager.availableResources);
    pthread_mutex_lock(&resourceManager.poolMutex);

    // Find a random available resource
    int resourceId = -1;
    while (resourceId == -1) {
        int randomResource = rand() % NUM_RESOURCES;
        if (resourceManager.resources[randomResource] == -1) {
            resourceId = randomResource;
            resourceManager.resources[resourceId] = id;
        }
    }

    // Release mutex and resource
    pthread_mutex_unlock(&resourceManager.poolMutex);
    printf("Thread %d is performing work with resource %d\n", id, resourceId + 1);

    // Simulate resting time
    sleep(2);

    // Release resource
    pthread_mutex_lock(&resourceManager.poolMutex);
    resourceManager.resources[resourceId] = -1;
    pthread_mutex_unlock(&resourceManager.poolMutex);
    sem_post(&resourceManager.availableResources);

    pthread_exit(NULL);
}

int main() {
    // Initialize resources and semaphore
    for (int i = 0; i < NUM_RESOURCES; ++i) {
        resourceManager.resources[i] = -1;
    }
    sem_init(&resourceManager.availableResources, 0, NUM_RESOURCES);
    pthread_mutex_init(&resourceManager.poolMutex, NULL);

    // Create threads
    pthread_t threads[NUM_THREADS];
    int threadIds[NUM_THREADS];
    for (int i = 0; i < NUM_THREADS; ++i) {
        threadIds[i] = i + 1;
        pthread_create(&threads[i], NULL, threadFunction, (void *)&threadIds[i]);
    }

    // Join threads
    for (int i = 0; i < NUM_THREADS; ++i) {
        pthread_join(threads[i], NULL);
    }

    // Clean up
    sem_destroy(&resourceManager.availableResources);
    pthread_mutex_destroy(&resourceManager.poolMutex);

    return 0;
}
