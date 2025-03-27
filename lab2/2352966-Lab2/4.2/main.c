#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#define NUM_MOVIES 1682

typedef struct
{
    char filename[256];
    double *result;
} threadData;

void *computeAverages(void *arg);

int main()
{
    threadData data1, data2;

    // Allocate memory for results for each thread
    data1.result = malloc(NUM_MOVIES * sizeof(double));
    data2.result = malloc(NUM_MOVIES * sizeof(double));
    if (!data1.result || !data2.result)
    {
        perror("malloc");
        return 1;
    }

    // Assign filenames to threads
    strcpy(data1.filename, "movie-100k_1.txt");
    strcpy(data2.filename, "movie-100k_2.txt");

    // Create threads
    pthread_t thread1, thread2;
    pthread_create(&thread1, NULL, computeAverages, &data1);
    pthread_create(&thread2, NULL, computeAverages, &data2);

    // Wait for both threads to complete
    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);

    // Write average ratings for each file
    if (freopen("output.txt", "w", stdout) == NULL)
    {
        perror("freopen");
        return 1;
    }

    printf("Averages from movie-100k_1.txt:\n");
    for (int i = 0; i < NUM_MOVIES; i++)
    {
        printf("Movie %d: %.2f\n", i + 1, data1.result[i]);
    }

    printf("\nAverages from movie-100k_2.txt:\n");
    for (int i = 0; i < NUM_MOVIES; i++)
    {
        printf("Movie %d: %.2f\n", i + 1, data2.result[i]);
    }

    // Free allocated memory
    free(data1.result);
    free(data2.result);

    return 0;
}

void *computeAverages(void *arg)
{
    threadData *data = (threadData *)arg;

    FILE *fp = fopen(data->filename, "r");
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
        data->result[i - 1] = counts[i] > 0 ? sums[i] / counts[i] : 0.0;
    }

    // Exit thread
    pthread_exit(NULL);
}