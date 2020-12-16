#include "omp.h"
#include <stdio.h>
#include <math.h>
#include <time.h> 
#include <stdlib.h>

#define INPUTS 15

/*
To run this program with single output mode enabled you must run the following command in the windows command prompt

set OMP_CANCELLATION=true

Bash command shell

export OMP_SCHEDULE="true"

then run with program.exe 1

*/

//Linked list data structure, this is used to ensure that each thread can independantly return its results
typedef struct linkedNode{
	int data;
	struct linkedNode* next;
}node;

typedef struct ll{
	node* first;
	node* last;
}linkedList;

void append(linkedList* ll, int val){
	node* newNode = (node*)malloc(sizeof(node));
	newNode->data = val;
	newNode->next = NULL;

	if(!ll->first){
		ll->first = newNode;
		ll->last = newNode;
	}
	else if(ll->first == ll->last){
		ll->last->next = newNode;
		ll->last = newNode;
		ll->first->next = ll->last;
	}
	else{
		ll->last->next = newNode;
		ll->last = newNode;
	}
}

void decToBinary(int num, int binValue[]) 
{
    int j = 0;
    for (j = 0; j < INPUTS; j++)
    {
        if ( num > 0 )
        {
            /*storing remainder in binary array*/
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
    /*each input has binary value, there are N inputs*/
    int combinations = pow(2, INPUTS) - 1;
    int results[combinations];
    int i = 0;
    int j = 0;
    int numThreads = omp_get_max_threads();
    int singleOutputFlag = 0;

    if(argc == 2){
    	singleOutputFlag = atoi(argv[1]);
    	printf("Single output mode enabled %d\n",omp_get_cancellation() );
    }
    /*This array has an index for each thread,*/
    /*Each index is an empty linked list*/
    linkedList* nodeArray[numThreads];
    for(i = 0; i < numThreads; i++){
    	nodeArray[i] = (linkedList*)malloc(sizeof(linkedList));
    	nodeArray[i]->first = NULL;
    	nodeArray[i]->last = NULL;
    }

    /*open file output.txt in write mode */
    FILE *fptr = fopen("circuitoutput.txt", "w"); 
    if (fptr == NULL) 
    { 
        printf("Could not open file"); 
        return 0; 
    }


    clock_t begin = clock();

    /*Call a open mp parallel for loop*/
    #pragma omp parallel
    {
    	#pragma omp for 
    		for (i = 0; i <= combinations; i++)
    		{
	    		/*declare local variables to avoid conflicting resource access*/
	    		int ID = omp_get_thread_num();
	    		int binValue[INPUTS];
		        decToBinary(i,binValue);
		        results[i] = validateCircuit(binValue);
		        /*This appends the result to the appropriate index in the nodeArray*/
		        if (results[i] == 1)
		        {
		        	if(singleOutputFlag >= 1){
		        		
		        		#pragma omp critical
		        		{
		        			if(singleOutputFlag == 1){
		        				for (j = 0; j < INPUTS; j++)
					        		fprintf(fptr, "%d", binValue[j]);
					    		fprintf(fptr,"\n");
		        			}
		        			singleOutputFlag++;
		        		}
		        		#pragma omp cancel for
		        		 
		        	}
		            append(nodeArray[ID],i);
		        }
   			}
    }
    	
    
    clock_t end = clock();

    /*Once all of the threads are finished iterate over the outputs*/
    if(singleOutputFlag == 0){
    	int binValue[INPUTS];
	    node* runner;
	    node* prevNode;
	    for(i = 0; i < numThreads; i++){
	    	runner = nodeArray[i]->first;
	    	while(runner != NULL){
	    		decToBinary(runner->data,binValue);
	    		for (j = 0; j < INPUTS; j++)
	        		fprintf(fptr, "%d", binValue[j]);
	    		fprintf(fptr,"\n");
	    		prevNode = runner;
	    		runner = runner->next;
	    		free(prevNode);
	    	}
	    	free(nodeArray[i]);
	    }
    }
    
    

    fclose(fptr);
    /* calculate elapsed time by finding difference (end - begin)*/
    printf("Time elpased is %.4f seconds", (double)(end - begin) / CLOCKS_PER_SEC);

    return 0;
}
