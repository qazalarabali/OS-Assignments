#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define BUFFER_SIZE 1024

// Structure to hold data shared between producer and consumer
typedef struct {
    FILE *input_file;
    FILE *output_file;
    char buffer[BUFFER_SIZE];
    size_t bytes_read;
    int buffer_empty;
    int buffer_full;
    pthread_mutex_t mutex;
    pthread_cond_t cond_producer;
    pthread_cond_t cond_consumer;
} SharedData;

// Function prototypes
void *producer(void *arg);
void *consumer(void *arg);

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s input_file output_file\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Open input file
    FILE *input_file = fopen(argv[1], "r");
    if (input_file == NULL) {
        perror("Error opening input file");
        exit(EXIT_FAILURE);
    }

    // Open output file
    FILE *output_file = fopen(argv[2], "w");
    if (output_file == NULL) {
        perror("Error opening output file");
        fclose(input_file);
        exit(EXIT_FAILURE);
    }

    // Initialize shared data
    SharedData shared_data;
    shared_data.input_file = input_file;
    shared_data.output_file = output_file;
    shared_data.bytes_read = 0;
    shared_data.buffer_empty = 1;
    shared_data.buffer_full = 0;
    pthread_mutex_init(&shared_data.mutex, NULL);
    pthread_cond_init(&shared_data.cond_producer, NULL);
    pthread_cond_init(&shared_data.cond_consumer, NULL);

    // Create producer and consumer threads
    pthread_t producer_thread, consumer_thread;
    pthread_create(&producer_thread, NULL, producer, (void *)&shared_data);
    pthread_create(&consumer_thread, NULL, consumer, (void *)&shared_data);

    // Wait for threads to finish
    pthread_join(producer_thread, NULL);
    pthread_join(consumer_thread, NULL);

    // Close files and destroy mutex/condition variables
    fclose(input_file);
    fclose(output_file);
    pthread_mutex_destroy(&shared_data.mutex);
    pthread_cond_destroy(&shared_data.cond_producer);
    pthread_cond_destroy(&shared_data.cond_consumer);

    return 0;
}

void *producer(void *arg) {
    SharedData *shared_data = (SharedData *)arg;

    while (!feof(shared_data->input_file)) {
        pthread_mutex_lock(&shared_data->mutex);

        // Wait for the consumer to consume the data
        while (shared_data->buffer_full) {
            pthread_cond_wait(&shared_data->cond_producer, &shared_data->mutex);
        }

        // Read part of the input file into the buffer
        shared_data->bytes_read = fread(shared_data->buffer, 1, BUFFER_SIZE, shared_data->input_file);

        // Signal that the buffer is full
        shared_data->buffer_full = 1;
        shared_data->buffer_empty = 0;

        pthread_cond_signal(&shared_data->cond_consumer);
        pthread_mutex_unlock(&shared_data->mutex);

        // Reset the buffer status
        shared_data->buffer_empty = 1;
    }

    // Signal the consumer that the producer has finished
    pthread_mutex_lock(&shared_data->mutex);
    shared_data->buffer_empty = 0;
    shared_data->buffer_full = 1;
    pthread_cond_signal(&shared_data->cond_consumer);
    pthread_mutex_unlock(&shared_data->mutex);

    pthread_exit(NULL);
}

void *consumer(void *arg) {
    SharedData *shared_data = (SharedData *)arg;

    while (1) {
        pthread_mutex_lock(&shared_data->mutex);

        // Wait for the producer to fill the buffer
        while (shared_data->buffer_empty) {
            pthread_cond_wait(&shared_data->cond_consumer, &shared_data->mutex);
        }

        // Write the buffer to the output file
        fwrite(shared_data->buffer, 1, shared_data->bytes_read, shared_data->output_file);

        // Signal that the buffer is empty
        shared_data->buffer_empty = 1;
        shared_data->buffer_full = 0;

        pthread_cond_signal(&shared_data->cond_producer);
        pthread_mutex_unlock(&shared_data->mutex);

        // Check if the producer has finished
        if (feof(shared_data->input_file))
            break;
    }

    pthread_exit(NULL);
}
