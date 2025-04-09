#ifndef PTHREAD_H
#include <pthread.h>
#endif

#include <stdatomic.h>

typedef struct pthread_seqlock
{
    /* TODO: implement the structure */
    _Atomic unsigned seq; // atomic sequence number
    pthread_mutex_t lock; // used by writer to serialize access
} pthread_seqlock_t;

static inline void pthread_seqlock_init(pthread_seqlock_t *rw)
{
    /* TODO: ... */
    atomic_init(&rw->seq, 0);
    pthread_mutex_init(&rw->lock, NULL);
}

static inline void pthread_seqlock_wrlock(pthread_seqlock_t *rw)
{
    /* TODO: ... */
    pthread_mutex_lock(&rw->lock);
    atomic_fetch_add(&rw->seq, 1); // Make it odd (write starts)
}

static inline void pthread_seqlock_wrunlock(pthread_seqlock_t *rw)
{
    /* TODO: ... */
    atomic_fetch_add(&rw->seq, 1); // seq becomes even (write ends)
    pthread_mutex_unlock(&rw->lock);
}

static inline unsigned pthread_seqlock_rdlock(pthread_seqlock_t *rw)
{
    /* TODO: ... */
    unsigned tmp;
    do
    {
        tmp = atomic_load(&rw->seq);
    } while (tmp % 2); // Wait until not writing
    return tmp;
}

static inline unsigned pthread_seqlock_rdunlock(pthread_seqlock_t *rw)
{
    /* TODO: ... */
    return atomic_load(&rw->seq); // Return the sequence number
}
