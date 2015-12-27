#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int check_prime(int n);

int main(int argc,char *argv[])
{
    unsigned int range = 10000001, i , count, tempi, prev_prime = range + 1;
    unsigned int start, chunksize;
    int f_prime, l_prime, *f_prime_arr, *l_prime_arr;
    int  myid, numprocs;
    int gap = 0, max_gap;
    int  namelen;
    char processor_name[MPI_MAX_PROCESSOR_NAME];

    double startwtime = 0.0, endwtime, totalwtime;

    MPI_Init(&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD,&numprocs);
    MPI_Comm_rank(MPI_COMM_WORLD,&myid);
    MPI_Get_processor_name(processor_name,&namelen);

    f_prime_arr = malloc(numprocs * sizeof(f_prime_arr));  	// Pointer arrays are able to be resized as the size of f_prime_arr
    l_prime_arr = malloc(numprocs * sizeof(l_prime_arr));       // and l_prime_arr will depend on numprocs

    startwtime = MPI_Wtime(); //*******************************************


    start = (myid*range/numprocs)+2; 				// index to start at deppending on the active node

    if (myid != numprocs -1)
        chunksize = range/numprocs; 				// chunksize for all node minus the last

    else
        chunksize = range - start; 				//chunksize for the last node [possible exception]

    i = start;

    /* Finds the first prime in the chunk */
    while (prev_prime == range + 1) {				//In line 10 prev_prime is set to the sentinel value range + 1
        if (check_prime(i) == 1){
            prev_prime = i;
                f_prime = i;  					//contains First prime for present active node
        }
        i++;
    }

    /* Finds the rest of the primes in a chunk, as well
       as the largest gap in the chunk */
    for (tempi = i; tempi < start + chunksize ; tempi++) {
    	if (tempi%2 == 0)
            continue;
        if (check_prime(tempi) == 1) {				 //Send target to check if it is prime;
            if (tempi - prev_prime > gap){
                gap = tempi - prev_prime;			 //Calculate the gap a pair of primes in active node.
            }
            prev_prime = tempi;

        }
    }
    l_prime = prev_prime; 					//Last prime for present active node.

    /* Sent the first and last primes of each chunk to proc#0, so it can
       determine the gap between them */
    MPI_Gather(&f_prime, 1, MPI_INT, f_prime_arr, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Gather(&l_prime, 1, MPI_INT, l_prime_arr, 1, MPI_INT, 0, MPI_COMM_WORLD);

    /* This segment works on the edges of the chunksize comparing the last
       prime of the previous array with the first of the next */
    if (myid == 0) {  							// This is ecxecuted sequentially by the first node
        for (count = 0; count < numprocs - 1; count++) {
            int mid_gap =  f_prime_arr[count+1] - l_prime_arr[count]; 	// mid_gap is the gap of the current pair of nodes i and i+1.
            if (mid_gap > gap) {
                gap = mid_gap;
            }
        }
    }

    /* Collect the biggest gap of each node and calculate the biggest
       overallgap */
    MPI_Reduce(&gap, &max_gap, 1, MPI_INT, MPI_MAX, 0, MPI_COMM_WORLD);


    endwtime = MPI_Wtime();   				//Stop counting time for active node.

    MPI_Reduce(&startwtime, &startwtime, 1, MPI_INT, MPI_MIN, 0, MPI_COMM_WORLD); //Collect the starttime of the first node to excecute
    MPI_Reduce(&endwtime, &endwtime, 1, MPI_INT, MPI_MAX, 0, MPI_COMM_WORLD);     //Collect the endtime of the last node to end


    MPI_Finalize();


    if(myid == 0){
        totalwtime = endwtime - startwtime; 		// Calculates the total time with the start time and end time of the nodes.
    	printf("Largest gap: %d\n", max_gap); 		// Displays the maximum gap of the primes
    	printf("Time elapsed: %f\n", totalwtime); 	// Displays the total running time
    }

    return 0;
}


/* This function verifies that the target value n is a prime number.
   Returns 0 for false or not prime and 1 for true or prime. */
int check_prime(int n) {
    int i;

    for (i = 2; i <= (int)sqrt(n); i++) {
        if (n%i == 0 && n!=i)
            return 0;
    }

    return 1;
}
