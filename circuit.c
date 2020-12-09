#include <stdio.h>
#include <math.h>
#include <time.h> 

#define INPUTS 15

// function to convert decimal to binary 
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

int validateCircuit(int binValue[])
{
    int result = 0;

    int A = binValue[0];
    int B = binValue[1];
    int C = binValue[2];
    int D = binValue[3];
    int E = binValue[4];
    int F = binValue[5];
    int G = binValue[6];
    int H = binValue[7];
    int I = binValue[8];
    int J = binValue[9];
    int K = binValue[10];
    int L = binValue[11];
    int M = binValue[12];
    int N = binValue[13];
    int O = binValue[14];

    result = (((G && H) && !I) && ((((A || B) && !C) || ((B && C) || ((D || E) && F))) && ((((L || M) && N) || (J && K)) && !O)));

    return result;
}

int main(int argc, char *argv[])
{
    //each input has binary value, there are N inputs
    int combinations = pow(2, INPUTS) - 1;
    int results[combinations];
    int i = 0;
    int j = 0;
    int binValue[INPUTS];

    //open file output.txt in write mode 
    FILE *fptr = fopen("circuitoutput.txt", "w"); 
    if (fptr == NULL) 
    { 
        printf("Could not open file"); 
        return 0; 
    }

    clock_t begin = clock();
    for (i = 0; i <= combinations; i++)
    {
        
        decToBinary(i,binValue);
        results[i] = validateCircuit(binValue);
        if (results[i] == 1)
        {
            for (j = 0; j < INPUTS; j++)
                fprintf(fptr, "%d", binValue[j]);
            fprintf(fptr,"\n");
        }
    }
    clock_t end = clock();
    
    fclose("circuitoutput.txt");
    // calculate elapsed time by finding difference (end - begin)
    printf("Time elpased is %.4f seconds", (double)(end - begin) / CLOCKS_PER_SEC);

    return 0;
}