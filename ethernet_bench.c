// ethernet_bench.c
//
// Measure nanoseconds spent inside the kernel for a single non-blocking packet read.
// Uses AF_PACKET/SOCK_RAW and recvfrom(MSG_DONTWAIT). Times only the syscall path.
//
// Build:
//   gcc ethernet_bench.c -o ethernet_bench
//
// Run (needs root or CAP_NET_RAW):
//   sudo ./ethernet_bench eth0 -n 2000000 -b 2048 -w 100
//
// Notes:
// - This measures recvfrom() latency (kernel entry->return) for a non-blocking socket.
// - "packet" means a successful recvfrom() returning >0 bytes.
// - "no packet" means EAGAIN/EWOULDBLOCK (nothing available).
// - Other errors are counted separately and excluded from success/empty stats by default.

#define _GNU_SOURCE
#include <errno.h>
#include <inttypes.h>
#include <linux/if_packet.h>
#include <linux/if_ether.h>
#include <net/ethernet.h>
#include <net/if.h>
#include <netinet/in.h>
#include <sched.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/resource.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

typedef struct
{
	uint64_t n;
	uint64_t sum_ns;
	uint64_t min_ns;
	uint64_t max_ns;
} stats_t;

static inline uint64_t ns_now_mono_raw(void)
{
	struct timespec ts;
	// CLOCK_MONOTONIC_RAW is best for measurement (no NTP slewing adjustments).
	if (clock_gettime(CLOCK_MONOTONIC_RAW, &ts) != 0)
	{
		perror("clock_gettime");
		exit(1);
	}
	return (uint64_t)ts.tv_sec * 1000000000ull + (uint64_t)ts.tv_nsec;
}

static inline void stats_init(stats_t *s)
{
	s->n = 0;
	s->sum_ns = 0;
	s->min_ns = UINT64_MAX;
	s->max_ns = 0;
}

static inline void stats_add(stats_t *s, uint64_t ns)
{
	s->n++;
	s->sum_ns += ns;
	if (ns < s->min_ns)
		s->min_ns = ns;
	if (ns > s->max_ns)
		s->max_ns = ns;
}

static void stats_print(const char *label, const stats_t *s)
{
	if (s->n == 0)
	{
		printf("%s: count=0\n", label);
		return;
	}
	double avg = (double)s->sum_ns / (double)s->n;
	printf("%s: count=%" PRIu64 "\n Min=%" PRIu64 " ns\n Max=%" PRIu64 " ns\n Avg=%.2f ns\n", label, s->n, s->min_ns, s->max_ns, avg);
}

static void pin_to_cpu0(void)
{
	cpu_set_t set;
	CPU_ZERO(&set);
	CPU_SET(0, &set);
	if (sched_setaffinity(0, sizeof(set), &set) != 0)
	{
        	// Not fatal; just reduces jitter if it works.
        	perror("sched_setaffinity");
	}
}

static void try_realtime_priority(void)
{
	struct sched_param sp;
	memset(&sp, 0, sizeof(sp));
	sp.sched_priority = 1; // low RT priority
	if (sched_setscheduler(0, SCHED_FIFO, &sp) != 0)
	{
        	// Not fatal; requires CAP_SYS_NICE or root.
        	// Keep quiet unless you want to see it:
        	// perror("sched_setscheduler");
	}
}

static void try_lock_memory(void)
{
	if (mlockall(MCL_CURRENT | MCL_FUTURE) != 0)
	{
		// Not fatal; reduces major jitter if it works.
		// perror("mlockall");
	}
}

static void usage(const char *prog)
{
	fprintf(stderr,
		"Usage: %s <ifname> [-n iterations] [-b buflen] [-w warmup]\n"
		"  <ifname>         Interface name (e.g., eth0)\n"
		"  -n iterations    Total recvfrom() attempts (default 1000000)\n"
		"  -b buflen        Receive buffer size (default 2048)\n"
		"  -w warmup        Warmup iterations excluded from stats (default 10000)\n",
		prog);
	exit(2);
}

int main(int argc, char **argv)
{
	if (argc < 2) usage(argv[0]);
		const char *ifname = argv[1];

	uint64_t iterations = 1000000;
	uint64_t warmup = 10000;
	size_t buflen = 2048;

	for (int i = 2; i < argc; i++)
	{
		if (!strcmp(argv[i], "-n") && i + 1 < argc)
		{
			iterations = strtoull(argv[++i], NULL, 10);
		}
		else if (!strcmp(argv[i], "-b") && i + 1 < argc)
		{
			buflen = (size_t)strtoull(argv[++i], NULL, 10);
		}
		else if (!strcmp(argv[i], "-w") && i + 1 < argc)
		{
			warmup = strtoull(argv[++i], NULL, 10);
		}
		else
		{
			usage(argv[0]);
		}
	}

	if (warmup > iterations)
		warmup = iterations;

	// Process tuning to reduce jitter.
	pin_to_cpu0();
	try_realtime_priority();
	try_lock_memory();

	// Allocate and fault-in receive buffer to avoid first-touch page faults during timing.
	uint8_t *buf = NULL;
	if (posix_memalign((void **)&buf, 64, buflen) != 0 || !buf)
	{
		fprintf(stderr, "posix_memalign failed\n");
		return 1;
	}
	memset(buf, 0, buflen);

	// Open raw packet socket (L2). Requires root/CAP_NET_RAW.
	int fd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
	if (fd < 0)
	{
		perror("socket(AF_PACKET,SOCK_RAW)");
		fprintf(stderr, "Need root or CAP_NET_RAW.\n");
		return 1;
	}

	// Bind to interface.
	unsigned int ifindex = if_nametoindex(ifname);
	if (ifindex == 0)
	{
		perror("if_nametoindex");
		return 1;
	}

	struct sockaddr_ll sll;
	memset(&sll, 0, sizeof(sll));
	sll.sll_family = AF_PACKET;
	sll.sll_protocol = htons(ETH_P_ALL);
	sll.sll_ifindex = (int)ifindex;

	if (bind(fd, (struct sockaddr *)&sll, sizeof(sll)) != 0)
	{
		perror("bind(AF_PACKET)");
		return 1;
	}

	// Optional: increase socket rcvbuf to reduce drops (not strictly required for timing).
	// Keep modest to avoid privilege failures; ignore errors.
	int rcvbuf = 4 * 1024 * 1024;
	setsockopt(fd, SOL_SOCKET, SO_RCVBUF, &rcvbuf, sizeof(rcvbuf));

	stats_t got_pkt, no_pkt;
	stats_init(&got_pkt);
	stats_init(&no_pkt);

	uint64_t other_err = 0;
	uint64_t bytes_total = 0;

	struct sockaddr_ll from;
	socklen_t fromlen = sizeof(from);

	// Main loop: recvfrom() with MSG_DONTWAIT. Measure only syscall.
	for (uint64_t i = 0; i < iterations; i++)
	{
		fromlen = sizeof(from);

		uint64_t t0 = ns_now_mono_raw();
		ssize_t r = recvfrom(fd, buf, buflen, MSG_DONTWAIT, (struct sockaddr *)&from, &fromlen);
		uint64_t t1 = ns_now_mono_raw();
		uint64_t dt = t1 - t0;

		if (i < warmup)
		{
			continue;
		}

		if (r > 0)
		{
			bytes_total += (uint64_t)r;
			stats_add(&got_pkt, dt);
		}
		else if (r < 0 && (errno == EAGAIN || errno == EWOULDBLOCK))
		{
			stats_add(&no_pkt, dt);
		}
		else
		{
			other_err++;
		}
	}

	printf("Interface: %s (ifindex=%u)\n", ifname, ifindex);
	printf("Iterations: %" PRIu64 " (warmup excluded: %" PRIu64 ")\n", iterations, warmup);
	printf("Buffer: %zu bytes\n", buflen);
	printf("Total bytes read (successful): %" PRIu64 "\n", bytes_total);
	printf("Other errors (excluded from stats): %" PRIu64 "\n", other_err);
	stats_print("recvfrom() returned PACKET", &got_pkt);
	stats_print("recvfrom() returned NO PACKET (EAGAIN)", &no_pkt);

	close(fd);
	free(buf);
	return 0;
}
