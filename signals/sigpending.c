/*
 * This code demonstrates how sigpending(2) works.  Run the program, send
 * SIGTERM to it, and then press Enter.  Try with a different signal.
 */

#define	_XOPEN_SOURCE	700

#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <err.h>
#include <assert.h>

#define	MESSAGE "SIGTERM caught !\n"

void
ctrl_c(int sig)
{
	write(1, MESSAGE, strlen(MESSAGE));
}

int
main(void)
{
	sigset_t sigset;
	struct sigaction act = { 0 };

	/* Block the SIGTERM first. */
	sigemptyset(&sigset);
	sigaddset(&sigset, SIGTERM);
	sigprocmask(SIG_BLOCK, &sigset, NULL);

	/* Install SIGTERM handler. */
	act.sa_handler = ctrl_c;
	sigaction(SIGTERM, &act, NULL);

	char buf[1];

	printf("%d: send me a SIGTERM, then pres Enter\n", getpid());
	if (read(0, buf, sizeof (buf)) == -1)
		err(1, "read");

	sigset_t freshset;
	sigpending(&freshset);
	assert(sigismember(&freshset, SIGTERM));
	printf("Yes, we got a pending SIGTERM.\n");
}
