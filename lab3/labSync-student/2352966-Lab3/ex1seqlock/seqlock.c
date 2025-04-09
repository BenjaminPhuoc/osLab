#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <pthread.h>

#include "seqlock.h" /* TODO implement this header file */

int val = 0;
pthread_seqlock_t lock;

void *writer(void *arg)
{
    pthread_seqlock_wrlock(&lock);
    val = 290405;
    printf("[Writer] val = %d (seq = %u)\n", val, lock.seq);
    pthread_seqlock_wrunlock(&lock);
    sleep(1); // Simulate work
    return NULL;
}

void *reader(void *arg)
{
    unsigned start, end;
    int local_val;
    do
    {
        start = pthread_seqlock_rdlock(&lock);
        local_val = val; // read shared data
        end = pthread_seqlock_rdunlock(&lock);
    } while (start != end); // Retry if write happened

    printf("[Reader] read val = %d (seq = %u)\n", local_val, start);
    sleep(1);
    return NULL;
}

int main()
{
    pthread_t t1, t2;

    pthread_seqlock_init(&lock);

    pthread_create(&t1, NULL, writer, NULL);
    pthread_create(&t2, NULL, reader, NULL);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    return 0;
}
