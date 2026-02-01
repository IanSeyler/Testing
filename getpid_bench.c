// gcc -o getpid_bench getpid_bench.c -lrt
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

int main()
{
	const int iterations = 1000000;
	struct timespec start, end;
	long long total_ns = 0;
	long long min_ns = 0;
	long long max_ns = 0;

	printf("Starting benchmark of getpid()...\n");

	for (int i = 0; i < iterations; i++)
	{
        	// Record start time
		clock_gettime(CLOCK_MONOTONIC, &start);

		// Call a kernel function
		getpid();

		// Record end time
		clock_gettime(CLOCK_MONOTONIC, &end);

		// Calculate the difference in nanoseconds
		long long delta = (end.tv_sec - start.tv_sec) * 1000000000LL + (end.tv_nsec - start.tv_nsec);

		// Update statistics
		total_ns += delta;
		if (delta < min_ns)
			min_ns = delta;
		if (delta > max_ns)
			max_ns = delta;
	}

	// Calculate average
	double avg_ns = (double)total_ns / iterations;

	// Display results
	printf("\n--- Results ---\n");
	printf("Iterations: %d\n", iterations);
	printf("Min: %lld ns\n", min_ns);
	printf("Max: %lld ns\n", max_ns);
	printf("Average: %.2f ns\n", avg_ns);

	return 0;
}
