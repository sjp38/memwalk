#include <locale.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>

#define ACCESS_ONCE(x) (*(volatile typeof(x) *)&(x))

#define TEN_TIMES(x)	\
	x;		\
	x;		\
	x;		\
	x;		\
	x;		\
	x;		\
	x;		\
	x;		\
	x;		\
	x;

char finish;

void init_mem(unsigned char *mem, unsigned long sz_mem)
{
	unsigned long i = 0;

	for (i = 0; i < sz_mem; i++)
		ACCESS_ONCE(mem[i]) = 1;
}

void *end_notice(void *runtime)
{
	usleep(*(unsigned long *)runtime * 1000 * 1000);
	finish = 1;

	return NULL;
}

/* mem should be initalized to have value 1 in each byte */
unsigned long walk_mem(unsigned char *mem, unsigned long sz_mem,
			unsigned long stride)
{
	unsigned long sum = 0;
	unsigned long i, j = 0;
	unsigned long suff_cycle;

	/* check ending signal once for loughly one billion memory accesses */
	suff_cycle = 1000 * 1000 * 1000 / (sz_mem / stride);

	while (1) {
		for (i = 0; i < sz_mem / stride - 10;) {
			TEN_TIMES(sum += ACCESS_ONCE(mem[i++ * stride]));
		}

		if (j++ % suff_cycle == 0 && ACCESS_ONCE(finish))
			break;
	}

	return sum;
}

int main(int argc, char *argv[])
{
	unsigned char *mem;
	unsigned long sz_mem;
	unsigned long stride;
	unsigned long runtime;

	unsigned long nr_accesses;
	struct timeval start, end;

	pthread_t end_notifier;

	if (argc < 4) {
		fprintf(stderr, "Usage: %s <memory size> <stride> <runtime>\n",
				argv[0]);
		exit(1);
	}
	setlocale(LC_NUMERIC, "");

	sz_mem = atol(argv[1]);
	stride = atol(argv[2]);
	runtime = atol(argv[3]);

	printf("Walk %'lu bytes array with %'lu bytes stride for %lu seconds\n",
			sz_mem, stride, runtime);

	if (gettimeofday(&start, NULL) != 0) {
		fprintf(stderr, "Failed to get init start time\n");
		return -1;
	}
	mem = (unsigned char *)malloc(sz_mem * sizeof(char));
	init_mem(mem, sz_mem);
	if (gettimeofday(&end, NULL) != 0) {
		fprintf(stderr, "Failed to get init end time\n");
		return -1;
	}

	printf("%'lu usecs for initalization\n",
			(end.tv_sec - start.tv_sec) * 1000000 +
			(end.tv_usec - start.tv_usec));

	if (gettimeofday(&start, NULL) != 0) {
		fprintf(stderr, "Failed to get start time\n");
		return -1;
	}

	pthread_create(&end_notifier, NULL, end_notice, &runtime);

	nr_accesses = walk_mem(mem, sz_mem, stride);

	if (gettimeofday(&end, NULL) != 0) {
		fprintf(stderr, "Failed to get end time\n");
		return -1;
	}

	printf("%'lu accesses per second, %'lu sec accesses\n",
			nr_accesses / (end.tv_sec - start.tv_sec),
			end.tv_sec - start.tv_sec);

	pthread_join(end_notifier, NULL);

	return 0;
}
