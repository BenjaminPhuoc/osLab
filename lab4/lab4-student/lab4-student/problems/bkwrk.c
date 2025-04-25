#define _GNU_SOURCE
#include "bktpool.h"
#include <signal.h>
#include <stdio.h>

#include <sched.h>
#include <sys/syscall.h> /* Definition of SYS_* constants */
#include <unistd.h>
#include <sys/mman.h>
#include <string.h>

// #define DEBUG
#define INFO
// #define WORK_THREAD

int bkwrk_worker(void *arg)
{
    sigset_t set;
    int sig;
    int s;
    int i = *((int *)arg); // Default arg is integer of workid

    struct bkworker_t *wrk = &worker[i];

    /* Taking the mask for waking up */
    sigemptyset(&set);
    sigaddset(&set, SIGUSR1);
    sigaddset(&set, SIGQUIT);

#ifdef DEBUG
    fprintf(stderr, "worker %i start living tid %d \n", i, getpid());
    fflush(stderr);
#endif

    while (1)
    {
        /* wait for signal */
        s = sigwait(&set, &sig);
        if (s != 0)
            continue;

#ifdef INFO
        fprintf(stderr, "worker wake %d up\n", i);
#endif

        /* Busy running */
        if (wrk->func != NULL)
            wrk->func(wrk->arg);

        /* Advertise I DONE WORKING */
        wrkid_busy[i] = 0;
        worker[i].func = NULL;
        worker[i].arg = NULL;
        worker[i].bktaskid = -1;
    }

    return 0;
}

int bktask_assign_worker(unsigned int bktaskid, unsigned int wrkid)
{
    if (wrkid < 0 || wrkid > MAX_WORKER)
        return -1;

    struct bktask_t *tsk = bktask_get_byid(bktaskid);

    if (tsk == NULL)
        return -1;

    /* Advertise I AM WORKING */
    wrkid_busy[wrkid] = 1;

    worker[wrkid].func = tsk->func;

#ifdef WORK_THREAD
    worker[wrkid].arg = tsk->arg;
#else
    shared_arg[wrkid] = *(int *)tsk->arg;
    worker[wrkid].arg = &shared_arg[wrkid];
#endif

    worker[wrkid].bktaskid = bktaskid;

    printf("Assign tsk %d wrk %d \n", tsk->bktaskid, wrkid);
    return 0;
}

int bkwrk_create_worker()
{
    unsigned int i;
    for (i = 0; i < MAX_WORKER; i++)
    {
#ifdef WORK_THREAD
        void **child_stack = (void **)malloc(STACK_SIZE);
        unsigned int wrkid = i;
        pthread_t threadid;

        sigset_t set;
        int s;

        sigemptyset(&set);
        sigaddset(&set, SIGQUIT);
        sigaddset(&set, SIGUSR1);
        sigprocmask(SIG_BLOCK, &set, NULL);

        /* Stack grow down - start at top*/
        void *stack_top = child_stack + STACK_SIZE;

        wrkid_tid[i] = clone(&bkwrk_worker, stack_top,
                             CLONE_VM | CLONE_FILES,
                             (void *)&i);
#ifdef INFO
        fprintf(stderr, "bkwrk_create_worker got worker %u\n", wrkid_tid[i]);
#endif

        usleep(100);

#else

        /* TODO: Implement fork version of create worker */
        sigset_t set;
        sigemptyset(&set);
        sigaddset(&set, SIGQUIT);
        sigaddset(&set, SIGUSR1);
        sigprocmask(SIG_BLOCK, &set, NULL);

        pid_t pid = fork();
        if (pid < 0)
        {
            perror("fork");
            return -1;
        }
        else if (pid == 0)
        {
            /* Child process: run worker loop */
            bkwrk_worker((void *)&i);
            _exit(0);
        }
        else
        {
            /* Parent process: record child PID */
            wrkid_tid[i] = pid;
#ifdef INFO
            fprintf(stderr, "bkwrk_create_worker got worker %u\n", wrkid_tid[i]);
#endif
            usleep(100);
        }
#endif
    }

    return 0;
}

int bkwrk_get_worker()
{
    /* TODO Implement the scheduler to select the resource entity
     * The return value is the ID of the worker which is not currently
     * busy or wrkid_busy[1] == 0
     */
    int start = wrkid_cur;
    for (int offset = 0; offset < MAX_WORKER; offset++)
    {
        int idx = (start + offset) % MAX_WORKER;
        if (wrkid_busy[idx] == 1)
            continue;

        /* reserve this worker for next time */
        wrkid_cur = (idx + 1) % MAX_WORKER;
        return idx;
    }
    /* no free worker found */
    return -1;
}

int bkwrk_dispatch_worker(unsigned int wrkid)
{

#ifdef WORK_THREAD
    unsigned int tid = wrkid_tid[wrkid];

    /* Invalid task */
    if (worker[wrkid].func == NULL)
        return -1;

#ifdef DEBUG
    fprintf(stderr, "brkwrk dispatch wrkid %d - send signal %u \n", wrkid, tid);
#endif

    syscall(SYS_tkill, tid, SIG_DISPATCH);
    // Wait so the worker can run the task and clear busy flag
    usleep(100);
    return 0;
#else
    /* TODO: Implement fork version to signal worker process here */
    pid_t pid = wrkid_tid[wrkid];

    /* Invalid task */
    if (worker[wrkid].func == NULL)
        return -1;

#ifdef DEBUG
    fprintf(stderr, "bkwrk dispatch wrkid %d - send signal %u \n", wrkid, pid);
#endif

    if (kill(pid, SIG_DISPATCH) < 0)
    {
        perror("kill");
        return -1;
    }
    /* Wait so the worker can run the task and clear busy flag */
    usleep(100);
    return 0;
#endif
}