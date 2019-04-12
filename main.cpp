#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <assert.h>
#include <pthread.h>
#include "ring_queue.h"

CRingQueue<unsigned int, 10> test_ringq;

void pin_cpu(int cpu_id)
{
	int s = 0;
    cpu_set_t cpuset;
    pthread_t thread;

    thread = pthread_self();

    /* Set affinity mask to include CPUs 0 to 7 */

    CPU_ZERO(&cpuset);
    CPU_SET(cpu_id, &cpuset);

    s = pthread_setaffinity_np(thread, sizeof(cpu_set_t), &cpuset);
    if (s != 0) {
        perror("pthread_setaffinity_np");
	}

    /* Check the actual affinity mask assigned to the thread */

    s = pthread_getaffinity_np(thread, sizeof(cpu_set_t), &cpuset);
    if (s != 0) {
    	perror("pthread_getaffinity_np");
	}
}

void *
func_enqueue(void *arg)
{
	pin_cpu(0);
	unsigned int i;
	unsigned int num = atoi((char *)arg);
	for (i = 1; i <= num; i++) {
		while(test_ringq.EnQueue(i) == false) {
		}
	}
	
	return NULL;
}

void *
func_dequeue(void *arg)
{
	pin_cpu(1);
	unsigned int i = 0;
	unsigned int item = 0;
	unsigned int num = atoi((char *)arg);
	for (i = 1; i <= num; i++) {
		while(test_ringq.DeQueue(item) == false) {
		}
		if (item != i) {
			printf("expect %u, read %u\n", i, item);
		}
		assert(item == i);
	}

	return NULL;
}

int main(int argc, char *argv[])
{
	pthread_t thd1, thd2;

	assert(argc == 2);

	printf ("enqueue %d\n", atoi(argv[1]));
	printf ("dequeue %d\n", atoi(argv[1]));
	pthread_create(&thd1, NULL, func_enqueue, argv[1]);
	pthread_create(&thd2, NULL, func_dequeue, argv[1]);
	pthread_join(thd1, NULL);
	pthread_join(thd2, NULL);
	
	assert(test_ringq.DeQueueAble() == false);

	return 0;
}
