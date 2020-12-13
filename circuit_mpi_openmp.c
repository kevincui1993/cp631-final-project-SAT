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

#define INPUTS 15
#define MIN(a,b) (((a)<(b))?(a):(b))

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

int validateCircuit(int binValue[]) {
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

void isCircuitSatisfied(int rank, int p, int combinations, int earlyExit)
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
        if (validateCircuit(binValue) == 1) {
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
    int combinations = pow(2, INPUTS) - 1;
    int m_rank;
    int p;
    int earlyExit = 0;

    if (argc == 2) {
        earlyExit = atoi(argv[1]);
    }

    /* Start up MPI */
    MPI_Init(&argc, &argv);

    /* Find out process rank  */
    MPI_Comm_rank(MPI_COMM_WORLD, &m_rank);

    /* Find out number of processes */
    MPI_Comm_size(MPI_COMM_WORLD, &p);

    clock_t begin = clock();

    isCircuitSatisfied(m_rank, p, combinations, earlyExit);

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