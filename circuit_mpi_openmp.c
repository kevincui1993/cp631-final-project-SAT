/*
mpicc -O2 circuit_mpi.c -o circuit_mpi.x
mpirun -np 4 ./circuit_mpi.x 1 (with early exit)
mpirun -np 4 ./circuit_mpi.x 0 (without early exit)
*/

#include <stdio.h>
#include <math.h>
#include <time.h> 
#include "mpi.h"
#include <string.h>
#include "logicalExpressionReader.h"

#define MIN(a,b) (((a)<(b))?(a):(b))

int INPUTS = 0;
// function to convert decimal to binary 
void decToBinary(int num, int binValue[]) {
    int j = 0;
    for (j = 0; j < INPUTS; j++) {
        if ( num > 0 ) {
            // storing remainder in binary array 
            binValue[j] = num % 2; 
            num = num / 2;
        } else {
            binValue[j] = 0;
        }
    }
}

int validateCircuit(int binValue[],char* expression, int expressionLength) {
    return evaluateExpression(expression, 0 , expressionLength, binValue, INPUTS);
}

void isCircuitSatisfied(int rank, int p, int combinations, int earlyExit, char* expression, int expressionLength)
{
    int blockLen = combinations / p + 1;
    int i = blockLen;
    int j = 0;
    int dest = 1;
    int localStart = 0;
    int isSatisfied = 0;
    int finalResult = 0;

    if (rank == 0) {
        for (dest = 1; dest < p; dest++) {
            MPI_Request req;
            MPI_Isend(&i, 1, MPI_INT, dest, 0, MPI_COMM_WORLD, &req);
            i += blockLen;
        }   
    }else {
        MPI_Status  status;  
        MPI_Recv(&localStart, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
    }

    //open file output.txt in write mode
    char filename[50] = "";
    snprintf(filename, sizeof filename, "circuitoutput_%d.txt", rank);
    FILE *fptr = fopen(filename, "w"); 
    if (fptr == NULL) { 
        printf("Could not open file"); 
        return; 
    }
    int k;
    #pragma omp for private(k, j)
    for (k = localStart ; k <= MIN(localStart + blockLen, combinations); k++) {
        
        int binValue[INPUTS];
        decToBinary(k,binValue);
        if (validateCircuit(binValue, expression, expressionLength) == 1) {
            #pragma omp critical
            {
                for (j = 0; j < INPUTS; j++)
                    fprintf(fptr, "%d", binValue[j]);
                fprintf(fptr, "\n");
                isSatisfied = 1;
            }
        }

        #pragma omp critical
        {
            if (finalResult == 0 && earlyExit != 0 && ((k-localStart + 1) % 100 == 0 || k == MIN(localStart + blockLen, combinations))) {
                //check if we found valid inputs, if so exit early
                MPI_Allreduce(&isSatisfied, &finalResult, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
                if (finalResult != 0) {
                    if (rank == 0) { 
                        printf("Found a solution, performing early exit\n");
                    }
                    #pragma omp cancel for
                }
            }
        }
    }
    fclose(fptr);
    if (earlyExit == 0) {
        MPI_Reduce(&isSatisfied, &finalResult, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
    }
    if (rank == 0) {
        printf("Circuit Satisfied: %s\n", finalResult != 0 ? "Yes" : "No");
    }
}

void combineOutputFiles(int rank, int p) {
    int i = 0;
    char c;
    if (rank == 0) {
        FILE *fptr = fopen("circuitoutput_mpi.txt", "w"); 
        if (fptr == NULL) { 
            printf("Could not open file"); 
            return; 
        }

        for (i = 0; i < p; i++) {
            char filename[50] = "circuitoutput_";
            snprintf(filename, sizeof filename, "circuitoutput_%d.txt", i);
            FILE *fsubPtr = fopen(filename, "r"); 
            if (fsubPtr == NULL) { 
                printf("Could not open file"); 
                return; 
            }
            while ((c = fgetc(fsubPtr)) != EOF) {
                fputc(c, fptr); 
            }
            fclose(fsubPtr);
            remove(filename);
        }
        fclose(fptr);
    }
}

int main(int argc, char *argv[])
{
    //each input has binary value, there are N inputs
    int combinations = 0;
    int m_rank;
    int p;
    int earlyExit = 0;
    int expressionLength;
    int bufferLength = 300;
    char buffer[bufferLength];

    if(argc != 3){
        printf("This program requires inputs\nThe first is either a 0 or 1 flag that triggers single output mode\n");
        printf("The second is the filename of the text file containing the circuit that will be evaluated\n");
        return 0;
    }

    earlyExit = atoi(argv[1]);
    /* Start up MPI */
    MPI_Init(&argc, &argv);

    /* Find out process rank  */
    MPI_Comm_rank(MPI_COMM_WORLD, &m_rank);

    /* Find out number of processes */
    MPI_Comm_size(MPI_COMM_WORLD, &p);

    if (m_rank == 0) {
        FILE *circuitFile = fopen(argv[2], "r"); 

        if(circuitFile == NULL){
            printf("please input a valid text file\n");
            return 0;
        }
        //Get the first line from the input
        fgets(buffer, bufferLength, circuitFile);
        //The first line will be a number
        INPUTS = atoi(buffer);
        //calculate the number of possible inputs
        combinations = pow(2, INPUTS) - 1;

        //The second line contains the circuit expression
        fgets(buffer, bufferLength, circuitFile);
        expressionLength = strlen(buffer);
        fclose(circuitFile);
    }
    MPI_Bcast(&INPUTS, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&combinations, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&expressionLength, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&buffer, expressionLength, MPI_CHAR, 0, MPI_COMM_WORLD);

    clock_t begin = clock();

    isCircuitSatisfied(m_rank, p, combinations, earlyExit, buffer, expressionLength);

    clock_t end = clock();
    
    MPI_Barrier(MPI_COMM_WORLD);
    // calculate elapsed time by finding difference (end - begin)
    if (m_rank == 0) {
        printf("Time elpased is %.4f seconds\n", (double)(end - begin) / CLOCKS_PER_SEC);
        combineOutputFiles(m_rank, p);
    }
    /* Shut down MPI */
    MPI_Finalize();
    return 0;
}