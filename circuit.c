#include <stdio.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include "logicalExpressionReader.h"

int INPUTS = 0;
/*function to convert decimal to binary */
void decToBinary(int num, int binValue[]) 
{
    int j = 0;
    for (j = 0; j < INPUTS; j++)
    {
        if ( num > 0 )
        {
            // storing remainder in binary array 
            binValue[j] = num % 2; 
            num = num / 2;
        }
        else
        {
            binValue[j] = 0;
        }
    }
}

int validateCircuit(int binValue[],char* expression, int expressionLength)
{
    return evaluateExpression(expression, 0 , expressionLength, binValue, INPUTS);
}

int main(int argc, char *argv[])
{
    /*each input has binary value, there are N inputs*/
    int combinations = 0; 
    int i = 0;
    int j = 0;
    char* expression;
    int expressionLength;
    int bufferLength = 300;
    char buffer[bufferLength];
    int finalResult = 0;

    if(argc != 3){
        printf("This program requires inputs\nThe first is either a 0 or 1 flag that triggers single output mode\n");
        printf("The second is the filename of the text file containing the circuit that will be evaluated\n");
        return 0;
    }
    /*open file output.txt in write mode */
    FILE *fptr = fopen("circuitoutput.txt", "w");
    /*Open filw with specified name*/
    FILE *circuitFile = fopen(argv[2], "r"); 

    if(circuitFile == NULL){
        printf("please input a valid text file\n");
        return 0;
    }

    if (fptr == NULL) 
    { 
        printf("Could not open file"); 
        return 0; 
    }

    //Get the first line from the input
    fgets(buffer, bufferLength, circuitFile);
    //The first line will be a number
    INPUTS = atoi(buffer);
    //calculate the number of possible inputs
    combinations = pow(2, INPUTS) - 1;
    //These variables must be declared here since input size was unkown beforehand
    int binValue[INPUTS];
    int* results = (int*)malloc(sizeof(int) * combinations);
    for(i = 0; i < combinations; i++){
        results[i] = 0;
    }
    //The second line contains the circuit expression
    fgets(buffer, bufferLength, circuitFile);
    expressionLength = strlen(buffer);
    expression = buffer;


    clock_t begin = clock();
    for (i = 0; i <= combinations; i++)
    {
        decToBinary(i,binValue);
        results[i] = validateCircuit(binValue,expression,expressionLength);
        if (results[i] == 1)
        {   
            finalResult = 1;
            for (j = 0; j < INPUTS; j++)
                fprintf(fptr, "%d", binValue[j]);
            fprintf(fptr,"\n");
            if(atoi(argv[1]) == 1){
                break;
            }
        }
    }
    clock_t end = clock();

    
    fclose(fptr);
    fclose(circuitFile);
    free(results);
    /*calculate elapsed time by finding difference (end - begin)*/
    printf("Circuit Satisfied: %s\n", finalResult != 0 ? "Yes" : "No");
    printf("Time elpased is %.4f seconds", (double)(end - begin) / CLOCKS_PER_SEC);

    return 0;
}