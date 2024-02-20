#include <stdio.h>

int main() {
    // File names in an array
    char *fileNames[] = {"datasetLR1.txt", "datasetLR2.txt", "datasetLR3.txt", "datasetLR4.txt"};

    // Declare variables
    int i, numLines = 0, x, y;
    float sumX = 0, sumY = 0, sumX2 = 0, sumY2 = 0, sumXY = 0;
    float coefficientA, coefficientB, predictedY, userInputX;

    // Loop through the files
    for (i = 0; i < 4; i++) {
        // Open the file
        FILE *filePointer = fopen(fileNames[i], "r");

        // Read the file
        while (fscanf(filePointer, "%d,%d", &x, &y) != EOF) {
            // Calculate the sums
            sumX += x;
            sumY += y;
            sumX2 += x * x;
            sumY2 += y * y;
            sumXY += x * y;
            numLines++;
        }

        // Close the file
        fclose(filePointer);
    }

    // Print the results
    printf("Total number of lines: %d\n", numLines);
    printf("Total sum of x: %f \n", sumX);
    printf("Total sum of y: %f \n", sumY);

    // Calculate coefficients using the formula
    coefficientA = ((sumY * sumX2) - (sumX * sumXY)) / ((numLines * sumX2) - (sumX * sumX));
    coefficientB = ((numLines * sumXY) - (sumX * sumY)) / ((numLines * sumX2) - (sumX * sumX));

    // Print the coefficients
    printf("Coefficient A: %f \n", coefficientA);
    printf("Coefficient B: %f \n", coefficientB);
    printf("Equation of line: y = %fx + %f \n", coefficientB, coefficientA);

    // Take input from the user
    printf("Enter value of x: ");
    scanf("%f", &userInputX);

    // Predict the value of y using the equation
    predictedY = (coefficientB * userInputX) + coefficientA;

    // Print the predicted value of y
    printf("The predicted value of y: %f\n", predictedY);
    
    printf("The program is executed successfully ");

    return 0;
}

