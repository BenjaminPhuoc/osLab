#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define BUFFER_SIZE 5
#define NUM_ITEMS 10

// Global shared data
int buffer[BUFFER_SIZE];
int count = 0;
// Mutex and two condition variables:
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_space = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_item = PTHREAD_COND_INITIALIZER;

int write_item(int item);
int read_item();

/* Producer thread routine */
void *producer(void *arg)
{
    for (int i = 0; i < NUM_ITEMS; i++)
    {
        write_item(i);
        // Sleep to simulate production delay
        sleep(1);
    }
    return NULL;
}

/* Consumer thread routine */
void *consumer(void *arg)
{
    for (int i = 0; i < NUM_ITEMS; i++)
    {
        read_item();
        // Sleep to simulate consumption delay
        sleep(1);
    }
    return NULL;
}

int main()
{
    pthread_t prod_thread, cons_thread;

    // Create producer and consumer threads
    pthread_create(&prod_thread, NULL, producer, NULL);
    pthread_create(&cons_thread, NULL, consumer, NULL);

    // Wait for both threads to finish
    pthread_join(prod_thread, NULL);
    pthread_join(cons_thread, NULL);

    // Clean up mutex and condition variables
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond_space);
    pthread_cond_destroy(&cond_item);

    return 0;
}

int write_item(int item)
{
    pthread_mutex_lock(&mutex);
    while (count == BUFFER_SIZE) // Buffer is full
    {
        pthread_cond_wait(&cond_space, &mutex);
    }
    // Add item to buffer
    buffer[count++] = item;
    printf("Produced: %d (count = %d)\n", item, count);

    // Notify reader that an item is available
    // TODO: enhance the mechanism to ensure this happens
    // when condition vars are locked
    pthread_cond_signal(&cond_item);
    pthread_mutex_unlock(&mutex);
    return 0;
}

int read_item()
{
    int item;
    pthread_mutex_lock(&mutex);
    while (count == 0) // Buffer is empty
    {
        pthread_cond_wait(&cond_item, &mutex);
    }
    // Retrive and get item from buffer
    item = buffer[--count];
    printf("Consumed: %d (count = %d)\n", item, count);

    // Notify writer that there is space in the buffer
    // TODO: enhance the mechanism to ensure this happens
    // when condition vars are locked
    pthread_cond_signal(&cond_space);
    pthread_mutex_unlock(&mutex);
    return item;
}