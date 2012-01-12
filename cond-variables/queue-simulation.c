/*
 * Queue simulation. At every step, the producer randomly generates a message
 * or not, and at each step, the consumer randomly takes a message or not. One
 * condition variable and one mutex is used. Note that if we used semaphores,
 * we would need 2 of them to represent both "sleeping" states.
 *
 * (c) jp@devnull.cz
 */

#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <poll.h>

#define	DEFAULT_MAX	10

int queue, max;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void *
producer(void *x)
{
	int rnd;

	while (1) {
		pthread_mutex_lock(&mutex);
		/* we can't insert a "message" when the queue is full */
		while (queue == max) {
			pthread_cond_wait(&cond, &mutex);
		}

		rnd = random() % 2;
		queue = queue + rnd;

		/*
		 * The queue was empty and we produced a "message". We must
		 * notify the consumer so it can start working on the queue
		 * again.
		 */
		if (queue == 1 && rnd == 1)
			pthread_cond_signal(&cond);

		pthread_mutex_unlock(&mutex);

		poll(NULL, 0, 95);
	}

	return (NULL);
}

void *
consumer(void *x)
{
	int rnd;

	while (1) {
		pthread_mutex_lock(&mutex);

		/* we can't get a "message" when there is none */
		while (queue == 0) {
			pthread_cond_wait(&cond, &mutex);
		}

		rnd = random() % 2;
		queue = queue - rnd;

		/*
		 * If the queue was full and we got a "message" from it we
		 * will signal the producer so that it can start producing
		 * messages again.
		 */
		if (queue == (max - 1) && rnd == 1)
			pthread_cond_signal(&cond);

		pthread_mutex_unlock(&mutex);

		poll(NULL, 0, 100);
	}
}


int
main(int argc, char **argv)
{
	int i;
	pthread_t t;

	if (argc == 2)
		max = atoi(argv[1]);
	else
		max = DEFAULT_MAX;

	/* Seed random device */
	/* XXX make it work universally on FreeBSD/Linux/OpenSolaris */
#ifdef __FreeBSD__
	srandomdev();
#endif

	pthread_create(&t, NULL, producer, NULL);
	pthread_create(&t, NULL, consumer, NULL);

	/*
	 * Main thread. Periodically print the "contents" of the queue.
	 */
	while (1) {
		pthread_mutex_lock(&mutex);
		printf(" |");
		for (i = 0; i < queue; ++i)
			putchar('.');
		for (i = queue; i < max; ++i)
			putchar(' ');
		printf("|\n");
		pthread_mutex_unlock(&mutex);
		poll(NULL, 0, 85);
	}

	/* never reached */
	return (0);
}
