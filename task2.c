#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

// Defining a structure to hold data for each thread
struct ThreadData {
    int start;
    int end;
    double sum;
};

void *calculatePi(void *arg) {
    struct ThreadData *data = (struct ThreadData *)arg;
    double sum = 0.0;
    int i;
    for (i = data->start; i <= data->end; i++) {
        double term = 1.0 / ((2 * i) + 1);
        if (i % 2 == 0) {
            sum += term;
        } else {
            sum -= term;
        }
    }
    // Storing the calculated sum in the thread data structure
    data->sum = sum;
    pthread_exit(NULL);
}

int main() {
    int iterations, numThreads;
    printf("Enter the number of iterations: ");    // taking user input for the number of iterations
    scanf("%d", &iterations);

    printf("Enter the number of threads: ");   // taking user input for the number of threads
    scanf("%d", &numThreads);

    // Array that holds thread identifiers
    pthread_t threads[numThreads];
    // Array that holds data for each thread
    struct ThreadData threadData[numThreads];

    // Calculating the number of iterations each thread will handle
    int iterationsPerThread = iterations / numThreads;
    // Calculating the remaining iterations not evenly distributed
    int remainingIterations = iterations % numThreads;

    int currentStart = 0;    // starting point for iteration
    int i;

    // Creating and launching threads
    for (i = 0; i < numThreads; i++) {
        // Assigning the start and end range of iterations to each thread
        threadData[i].start = currentStart;
        threadData[i].end = currentStart + iterationsPerThread - 1;

        // Adjusting the end range for any remaining iterations
        if (remainingIterations > 0) {
            threadData[i].end++;
            remainingIterations--;
        }

        // Updating the starting point for the next thread
        currentStart = threadData[i].end + 1;

        // Creating a thread and call the calculatePi function
        pthread_create(&threads[i], NULL, calculatePi, &threadData[i]);
    }

    // Waiting for all threads to finish and accumulate their results
    double pi = 0.0;
    for (i = 0; i < numThreads; i++) {
        pthread_join(threads[i], NULL);
        pi += threadData[i].sum;
    }
    // Multiplying the accumulated sum by 4 to get an approximate value of Pi
    pi *= 4.0;
    // Printing the result
    printf("Value of Pi after %d iterations is: %.15lf\n", iterations, pi);

    return 0;
}
