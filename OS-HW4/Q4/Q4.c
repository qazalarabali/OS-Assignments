#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>
#include <unistd.h>

#define NUMBER_OF_RESOURCES 5
#define NUMBER_OF_THREAD 10

typedef struct {
    int resources[NUMBER_OF_RESOURCES];
    sem_t availableResources;
    pthread_mutex_t poolMutex;
} ResourceManager;

ResourceManager RManger;

void* threadFunc(void* arg) 
{
    int threadNum = *(int*)arg;
    sem_wait(&RManger.availableResources); // Wait for available resource
    int resource_avail = -1;
    pthread_mutex_lock(&RManger.poolMutex); // Acquire lock on resource pool
    for (int i = 0; i < NUMBER_OF_RESOURCES; i++) 
    {
        if (RManger.resources[i] == -1) 
        { 
            // Find an available resource
            RManger.resources[i] = threadNum; // Assign the resource to the current thread
            resource_avail = i;
            break;
        }
    }
    pthread_mutex_unlock(&RManger.poolMutex); // Release lock on resource pool

    srand(time(NULL));
    printf("Thread %d is performing work with resource %d\n", threadNum, resource_avail + 1);
    sleep(rand() % 10); // Simulate work being done

    pthread_mutex_lock(&RManger.poolMutex); // Acquire lock on resource pool
    RManger.resources[resource_avail] = -1; // Release the resource
    pthread_mutex_unlock(&RManger.poolMutex); // Release lock on resource pool

    sem_post(&RManger.availableResources); // Release the resource
    free(arg);
    pthread_exit(NULL);
}

int main() 
{
    pthread_t threads[NUMBER_OF_THREAD];
    pthread_mutex_init(&RManger.poolMutex, NULL);
    sem_init(&RManger.availableResources, 0, NUMBER_OF_RESOURCES); // Initialize semaphore with the number of resources

    for (int i = 0; i < NUMBER_OF_RESOURCES; i++) 
    {
        RManger.resources[i] = -1; // Initialize resources as unallocated
    }

    for (int i = 0; i < NUMBER_OF_THREAD; i++) 
    {
        int* threadNum = malloc(sizeof(int));
        *threadNum = i + 1;
        pthread_create(&threads[i], NULL, threadFunc, threadNum); // Create threads
    }

    for (int i = 0; i < NUMBER_OF_THREAD; i++) 
    {
        pthread_join(threads[i], NULL); // Wait for threads to finish
    }

    return 0;
}