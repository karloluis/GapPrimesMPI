Gaps between consecutive primes
	by Cassandra Schaening-Burgos
	and Karlo Martinez Martos

prime_gap_FINAL.c

Determines the gap between all consecutive primes in the range of integers from
1 to 10.000.001. It does this in parallel by assigning a chunk of contiguous 
integers to each processor, which then determines the largest gap in its range.
A separate loop determines the length of the gap between the last prime of a 
chunk and the first of the following chunk.

Compile with:
	$ mpicc prime_gap_FINAL.c -o prime_gap

Run as:
	$ mpirun -np <numprocs> prime_gap
        Where <numprocs> is the number of processor to be used.
	This must be done on a cluster or computer with multiple processors and
	support for MPI.


