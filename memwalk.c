#include <locale.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
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

	while (1) {
		for (i = 0; i < sz_mem / stride - 10;) {
			TEN_TIMES(sum += ACCESS_ONCE(mem[i++ * stride]));
		}

		if (j++ % 100 == 0 && ACCESS_ONCE(finish))
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

	mem = (unsigned char *)malloc(sz_mem * sizeof(char));
	init_mem(mem, sz_mem);

	pthread_create(&end_notifier, NULL, end_notice, &runtime);

	printf("%'lu accesses per second\n",
			walk_mem(mem, sz_mem, stride) / runtime);

	pthread_join(end_notifier, NULL);

	return 0;
}
