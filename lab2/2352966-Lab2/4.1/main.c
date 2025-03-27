#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <unistd.h>

#define NUM_MOVIES 1682

void computeAverages(const char *filename, double *sharedDataSlot);

int main()
{
    size_t sharedDataSize = 2 * NUM_MOVIES * sizeof(double);
    int sharedDataID = shmget(0x123, sharedDataSize, IPC_CREAT | 0666);
    if (sharedDataID < 0)
    {
        perror("shmget");
        return 1;
    }

    // Attach shared memory in the parent
    double *pSharedData = (double *)shmat(sharedDataID, NULL, 0);
    if (pSharedData == (void *)-1)
    {
        perror("shmat");
        return 1;
    }

    // Fork first child to process movie-100k_1.txt (slot 0)
    pid_t pid1 = fork();
    if (pid1 < 0)
    {
        perror("fork child1");
        return 1;
    }
    if (pid1 == 0)
    {
        // Child process 1
        double *slot = pSharedData; // slot 0
        computeAverages("movie-100k_1.txt", slot);
        // Detach shared memory and exit
        if (shmdt(pSharedData) == -1)
            perror("shmdt child1");
        return 0;
    }

    // Fork second child to process movie-100k_2.txt (slot 1)
    pid_t pid2 = fork();
    if (pid2 < 0)
    {
        perror("fork child2");
        return 1;
    }
    if (pid2 == 0)
    {
        // Child process 2
        double *slot = pSharedData + NUM_MOVIES; // slot 1 (offset by NUM_MOVIES)
        computeAverages("movie-100k_2.txt", slot);
        // Detach shared memory and exit
        if (shmdt(pSharedData) == -1)
            perror("shmdt child2");
        return 0;
    }

    // Parent process: wait for both children to finish
    wait(NULL);
    wait(NULL);

    // Write average ratings for each file
    if (freopen("output.txt", "w", stdout) == NULL)
    {
        perror("freopen");
        return 1;
    }

    printf("Averages from movie-100k_1.txt:\n");
    for (int i = 0; i < NUM_MOVIES; i++)
    {
        printf("Movie %d: %.2f\n", i + 1, pSharedData[i]);
    }

    printf("\nAverages from movie-100k_2.txt:\n");
    for (int i = 0; i < NUM_MOVIES; i++)
    {
        printf("Movie %d: %.2f\n", i + 1, pSharedData[NUM_MOVIES + i]);
    }

    // Detach shared memory in the parent and mark it for removal
    if (shmdt(pSharedData) == -1)
    {
        perror("shmdt parent");
        return 1;
    }
    if (shmctl(sharedDataID, IPC_RMID, NULL) == -1)
    {
        perror("shmctl");
        return 1;
    }

    return 0;
}

void computeAverages(const char *filename, double *sharedDataSlot)
{
    FILE *fp = fopen(filename, "r");
    if (!fp)
    {
        perror("fopen");
        exit(1);
    }

    double sums[NUM_MOVIES + 1] = {0}; // Using 1-index for movieID (movie IDs: 1..1682)
    int counts[NUM_MOVIES + 1] = {0};

    char buffer[256];
    while (fgets(buffer, sizeof(buffer), fp))
    {
        int userID, movieID, timeStamp;
        double rating;
        // Parse the line; expect 4 values separated by tab characters
        if (sscanf(buffer, "%d\t%d\t%lf\t%d", &userID, &movieID, &rating, &timeStamp) == 4)
        {
            sums[movieID] += rating;
            counts[movieID]++;
        }
    }
    fclose(fp);

    // Compute average rating for each movie (if there is at least one rating)
    for (int i = 1; i <= NUM_MOVIES; i++)
    {
        sharedDataSlot[i - 1] = counts[i] > 0 ? sums[i] / counts[i] : 0.0;
    }
}