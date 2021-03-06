/*
 * Note that when fork(2) is used from a thread that is not main() then such
 * a thread can live in the child process as long as it wants to since there is
 * no main() thread to exit - the main function from then on is the thread
 * function itself now.
 *
 * (by) jp@devnull.cz
 */

#include <err.h>
#include <pthread.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

#define	NLOOPS	3

void *
thread(void *x)
{
	pid_t pid;

	printf("%d: before fork() in non-main thread\n", getpid());
	if ((pid = fork()) == -1)
		err(1, "fork");

	if (pid == 0)
		printf("%d: I'm child thread()\n", getpid());
	else
		printf("%d: I'm parent thread()\n", getpid());

	for (int i = 0; i < NLOOPS; ++i) {
		printf("%d: loop #%d\n", getpid(), i);
		sleep(1);
	}
	return (NULL);
}

int
main(void)
{
	pthread_t t;

	(void) printf("%d: in main: starting.\n", getpid());
	(void) pthread_create(&t, NULL, thread, NULL);
	(void) pthread_join(t, NULL);
	(void) printf("%d: in main: exiting.\n", getpid());
}
