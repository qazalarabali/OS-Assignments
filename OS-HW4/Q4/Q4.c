#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

typedef struct {
    int resources[5];
    sem_t availableResources;
    pthread_mutex_t poolMutex;
} ResourceManager;

int main() {
    ResourceManager resourceManager;
    for (int i = 0; i < 5; i++) {
        resourceManager.resources[i] = -1;
    }
    
    // Rest of your code...

    return 0;
}