// gcc -o l_bench l_bench.c -lrt
#include <stdio.h>
#include <unistd.h>
#include <time.h>

int main()
{
	const int iterations = 1000000;
	struct timespec start, end;
	long long total_ns = 0;

	printf("Starting benchmark...\n");

	for (int i = 0; i < iterations; i++)
	{
        	// Record start time
		clock_gettime(CLOCK_MONOTONIC, &start);

		//-------------------------
		/* Code to benchmark */
		//-------------------------
		// Call a kernel function
		// getpid();
		//-------------------------
		// __asm__ volatile ("nop");
		//-------------------------
		__asm__ volatile (
			"xor %%eax, %%eax;"
			"xor %%ecx, %%ecx;"
			"cpuid"
			: // output
			: // input
			: "%eax", "%ebx", "%ecx", "%edx" // clobbered registers
		);
		//-------------------------

		// Record end time
		clock_gettime(CLOCK_MONOTONIC, &end);

		// Calculate the difference in nanoseconds
		long long delta = (end.tv_sec - start.tv_sec) * 1000000000LL + (end.tv_nsec - start.tv_nsec);

		// Update statistics
		total_ns += delta;
	}

	// Calculate average
	double avg_ns = (double)total_ns / iterations;

	// Display results
	printf("\n--- Results ---\n");
	printf("Iterations: %d\n", iterations);
	printf("Average: %.2f ns\n", avg_ns);

	return 0;
}
