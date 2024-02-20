#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

//initializing the mutex
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

//file pointer to write the output to the file
FILE *filePointer;

//pointer to the array of integers
int *pointer;

//count to count the number of prime numbers
int count = 0;

//structure to store the start and end index of the array
struct prime {
    int start;
    int end;
};

//function to check whether the number is prime or not
int checkPrime(int num) {
    int i;
    if (num == 0 || num == 1) {
        return 0;
    }
    for (i = 2; i <= num / 2; i++) {
        if (num % i == 0) {
            return 0;
        }
    }
    return 1;
}

//function to be executed by the threads
void *threadFunc(void *p) {
    filePointer = fopen("output_file.txt", "w");
    int i;
    //typecasting the void pointer to the structure pointer
    struct prime *pr = (struct prime *)p;

    //loop to check the prime numbers in the array
    for (i = pr->start; i <= pr->end; i++) {
        //calling the function to check whether the number is prime or not
        int check = checkPrime(pointer[i]);
        
        //locking the mutex
        pthread_mutex_lock(&mutex);

        //writing the prime numbers to the file
        if (check) {
            fprintf(filePointer, "%d\n", pointer[i]);
            count++;
        }

        //unlocking the mutex
        pthread_mutex_unlock(&mutex);
    }
}

int main() {
    //initializing the variables
    int sum = 0;
    int numThread;
    int chars, j;

    //opening the files and reading the data from the files
    char *f[] = {"PrimeData1.txt", "PrimeData2.txt", "PrimeData3.txt"};
    for (j = 0; j < 3; j++) {
        FILE *fp = fopen(f[j], "r");
        while (fscanf(fp, "%d", &chars) != EOF) {
            sum++;
        }
        fclose(fp);
    }

    //initializing the variable
    int i = 0;

    //asking the user to enter the number of threads
    printf("Enter the number of threads : ");
    scanf("%d", &numThread);

    //allocating memory to the pointer
    pointer = (int *)malloc(sum * sizeof(int));

    //opening the files and reading the data from the files
    for (j = 0; j < 3; j++) {
        FILE *fp = fopen(f[j], "r");
        // Reading the data from the file and storing it in the array
        while (fscanf(fp, "%d", &pointer[i]) != EOF) {
            i++;
        }
        fclose(fp);
    }

    // Calculating the number of elements to be processed by each thread
    int temp, slice[numThread];
    // Calculating the number of elements to be processed by each thread
    temp = sum % numThread;
    // Calculating the number of elements to be processed by each thread
    int startThread[numThread];
    int endThread[numThread];
    for (i = 0; i < numThread; i++) {
        slice[i] = sum / numThread;
    }
    for (i = 0; i < temp; i++) {
        slice[i] = slice[i] + 1;
    }

    // Calculating the start and end index of the array to be processed by each thread
    for (i = 0; i < numThread; i += 1) {
        if (i == 0) {
            startThread[i] = 0;
        } else {
            startThread[i] = endThread[i - 1] + 1;
        }
        endThread[i] = startThread[i] + slice[i] - 1;
    }

    // Array of structure to store the start and end index of the array to be processed by each thread
    struct prime ptrTh[numThread];

    for (i = 0; i < numThread; i++) {
        ptrTh[i].start = startThread[i];
        ptrTh[i].end = endThread[i];
    }

    pthread_t threads[numThread];
    for (i = 0; i < numThread; i++) {
        // Creating the threads
        pthread_create(&threads[i], NULL, threadFunc, &ptrTh[i]);
    }

    // Joining the threads
    for (i = 0; i < numThread; i++) {
        pthread_join(threads[i], NULL);
    }

    // Printing the total number of prime numbers
    printf("Total Prime Numbers :  %d", count);

    // Closing the file
    free(pointer);
    fclose(filePointer);

    return 0;
}

