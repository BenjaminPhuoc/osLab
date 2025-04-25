#include "bktpool.h"
#include <sys/mman.h>

int bktpool_init()
{
#ifndef WORK_THREAD
    /* allocate shared arrays so parent and forks see the same memory */
    wrkid_tid = mmap(NULL, MAX_WORKER * sizeof(*wrkid_tid), PROT_READ | PROT_WRITE,
                     MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    wrkid_busy = mmap(NULL, MAX_WORKER * sizeof(*wrkid_busy), PROT_READ | PROT_WRITE,
                      MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    worker = mmap(NULL, MAX_WORKER * sizeof(*worker), PROT_READ | PROT_WRITE,
                  MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    shared_arg = mmap(shared_arg, MAX_WORKER * sizeof *shared_arg, PROT_READ | PROT_WRITE,
                      MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    if (wrkid_tid == MAP_FAILED || wrkid_busy == MAP_FAILED ||
        worker == MAP_FAILED || shared_arg == MAP_FAILED)
        return -1;
#endif
    return bkwrk_create_worker();
}