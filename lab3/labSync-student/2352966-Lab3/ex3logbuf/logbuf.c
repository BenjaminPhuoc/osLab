#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>

#define MAX_LOG_LENGTH 10
#define MAX_BUFFER_SLOT 6
#define MAX_LOOPS 30

char logbuf[MAX_BUFFER_SLOT][MAX_LOG_LENGTH];

int count;
void flushlog();

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

struct _args
{
    unsigned int interval;
};

void *wrlog(void *data)
{
    char str[MAX_LOG_LENGTH];
    int id = *(int *)data;

    sleep(1);
    sprintf(str, "%d", id);

    pthread_mutex_lock(&lock); // Lock the mutex
    // Wait if buffer is full
    while (count >= MAX_BUFFER_SLOT)
    {
        pthread_cond_wait(&cond, &lock); // release mutex and wait
    }

    // Write log entry
    strcpy(logbuf[count], str);
    count++;
    pthread_mutex_unlock(&lock); // Unlock the mutex

    return 0;
}

void flushlog()
{
    int i;
    char nullval[MAX_LOG_LENGTH];

    pthread_mutex_lock(&lock); // Lock the mutex
    sprintf(nullval, "%d", -1);
    for (i = 0; i < count; i++)
    {
        printf("Slot  %i: %s\n", i, logbuf[i]);
        strcpy(logbuf[i], nullval);
    }
    /*Reset buffer */
    count = 0;
    // Signal all waiting writer threads
    pthread_cond_broadcast(&cond);
    pthread_mutex_unlock(&lock); // Unlock the mutex

    fflush(stdout);

    return;
}

void *timer_start(void *args)
{
    while (1)
    {
        flushlog();
        /*Waiting until the next timeout */
        sleep(((struct _args *)args)->interval);
    }
}

int main()
{
    int i;
    count = 0;
    pthread_t tid[MAX_LOOPS];
    pthread_t lgrid;
    int id[MAX_LOOPS];

    struct _args args;
    args.interval = 1;
    /*500 msec ~ 500 * 1000 usec */

    /*Setup periodically invoke flushlog() */
    pthread_create(&lgrid, NULL, &timer_start, (void *)&args);

    /*Asynchronous invoke task writelog */
    for (i = 0; i < MAX_LOOPS; i++)
    {
        id[i] = i;
        pthread_create(&tid[i], NULL, wrlog, (void *)&id[i]);
    }

    for (i = 0; i < MAX_LOOPS; i++)
        pthread_join(tid[i], NULL);

    sleep(5);

    // Clean up
    pthread_mutex_destroy(&lock);

    return 0;
}
