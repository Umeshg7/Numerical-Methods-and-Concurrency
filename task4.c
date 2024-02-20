#include <stdio.h>
#include "lodepng.h"
#include <stdlib.h>
#include <pthread.h>

int ***pixelArray; //3D array to store pixel values of the image
unsigned char *inputImage; //array to store the decoded image

//structure to hold arguments for the thread
struct ThreadArguments {
    int startHeight;
    int endHeight;
    int imageHeight;
    int imageWidth;
    int threadID;
    int kernelSize;
};

//function to apply Gaussian blur on a portion of the image
void *applyBlur(void *arguments) {
    struct ThreadArguments *args = (struct ThreadArguments *)arguments;
    int startH = args->startHeight, endH = args->endHeight, kernelSize = args->kernelSize;
    int threadID = args->threadID, height = args->imageHeight, width = args->imageWidth;

    for (int i = startH; i <= endH; i++) {
        for (int j = 0; j < width; j++) {
            int avgR = 0, avgG = 0, avgB = 0, numOfPixels = 0;

            for (int l = -kernelSize; l <= kernelSize; l += kernelSize) {
                for (int m = -kernelSize; m <= kernelSize; m += kernelSize) {
                    int xPixel = i + l;
                    int yPixel = j + m;

                    if (xPixel >= 0 && yPixel >= 0 && xPixel < height && yPixel < width) {
                        avgR += pixelArray[xPixel][yPixel][0];
                        avgG += pixelArray[xPixel][yPixel][1];
                        avgB += pixelArray[xPixel][yPixel][2];
                        numOfPixels++;
                    }
                }
            }

            //updating the blurred pixel values in the input image
            inputImage[4 * width * i + 4 * j + 0] = avgR / numOfPixels;
            inputImage[4 * width * i + 4 * j + 1] = avgG / numOfPixels;
            inputImage[4 * width * i + 4 * j + 2] = avgB / numOfPixels;
        }
    }

    pthread_exit(0);
}

int main() {
    unsigned int decodingError, imgWidth, imgHeight;
    int i, j, k;

    int numThreads, kernelSize;
    char fileName[20];

    //reading the file name from the user
    printf("Enter the file that you want to blur: \n");
    scanf("%s", fileName);

    //decoding the image using LodePNG
    decodingError = lodepng_decode32_file(&inputImage, &imgWidth, &imgHeight, fileName);

    //getting user input for the number of threads
    printf("Please enter the number of threads that you want to use to blur the image: ");
    scanf("%d", &numThreads);

    //adjusting the number of threads if it exceeds the image height
    if (numThreads > imgHeight) {
        numThreads = imgHeight % numThreads + 1;
        printf("Invalid number of threads. Automatically choosing the appropriate number of threads: %d threads\n", numThreads);
    }

    //getting user input for the size of the blur kernel
    printf("Please enter the size of the Kernel(blur matrix) that you want to use Enter '1' for a 3x3 matrix, '2' for 5x5, '3' for 7x7 and so on: ");
    scanf("%d", &kernelSize);

    //allocating memory for the 3D pixel array
    pixelArray = (int ***)malloc(imgHeight * sizeof(int **));
    for (i = 0; i < imgHeight; i++) {
        pixelArray[i] = (int **)malloc(imgWidth * sizeof(int *));
        for (j = 0; j < imgWidth; j++) {
            pixelArray[i][j] = (int *)malloc(3 * sizeof(int));
        }
    }

    //checking for decoding errors
    if (decodingError) {
        printf("Error in decoding image %d: %s\n", decodingError, lodepng_error_text(decodingError));
    } else {
        //storing the decoded image in the pixel array
        for (i = 0; i < imgHeight; i++) {
            for (j = 0; j < imgWidth; j++) {
                for (k = 0; k < 3; k++) {
                    pixelArray[i][j][k] = inputImage[4 * imgWidth * i + 4 * j + k];
                }
            }
        }
    }

    int sliceHeight[numThreads];

    //calculating slice height for each thread
    for (i = 0; i < numThreads; i++) {
        sliceHeight[i] = imgHeight / numThreads;
    }

    //adjusting slice height for any remaining pixels
    int reminderHeight = imgHeight % numThreads;
    for (i = 0; i < reminderHeight; i++) {
        sliceHeight[i]++;
    }

    //calculating start and end height for each thread
    int startHeight[numThreads], endHeight[numThreads];
    for (i = 0; i < numThreads; i++) {
        if (i == 0) {
            startHeight[i] = 0;
        } else {
            startHeight[i] = endHeight[i - 1] + 1;
        }
        endHeight[i] = startHeight[i] + sliceHeight[i] - 1;
    }

    struct ThreadArguments threadArgs[numThreads];
    pthread_t threads[numThreads];

    int threadID = 1;

    //creating threads to apply blur concurrently
    for (i = 0; i < numThreads; i++) {
        threadArgs[i].startHeight = startHeight[i];
        threadArgs[i].endHeight = endHeight[i];
        threadArgs[i].imageHeight = imgHeight;
        threadArgs[i].imageWidth = imgWidth;
        threadArgs[i].threadID = threadID;
        threadArgs[i].kernelSize = kernelSize;
        threadID++;
        pthread_create(&threads[i], NULL, applyBlur, &threadArgs[i]);
    }

    //joining threads to wait for their completion
    for (i = 0; i < numThreads; i++) {
        pthread_join(threads[i], NULL);
    }

    unsigned char *outputPNG;
    char outputImageName[20];
    size_t outputPNGSize;

    //getting user input for the output image name
    printf("Image has been blurred. Please enter the name of the output image: ");
    scanf("%s", outputImageName);

    //encoding and saving the blurred image using LodePNG
    lodepng_encode32(&outputPNG, &outputPNGSize, inputImage, imgWidth, imgHeight);
    lodepng_save_file(outputPNG, outputPNGSize, outputImageName);

    return 0;
}

