/*
 * See if there is a way how to determine if file descriptor points to stdout.
 * Run it like this:
 *
 *   ./a.out
 *   ./a.out >/dev/null
 *   ./a.out 2>/dev/null
 *
 * Verify with 'ls -aldi /dev/null' that the long inode number reported
 * in the last step belongs to /dev/null.
 *
 * (c) vlada@devnull.cz
 */

#define	_POSIX_C_SOURCE	2

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#ifdef linux
#include <sys/sysmacros.h>
#endif
#include <fcntl.h>
#include <unistd.h>
#include <err.h>

void
statfd(int fd)
{
	struct stat st;

	if (fstat(fd, &st) == -1)
		err(1, "fstat");
	printf("fd %d (tty: %s)\n", fd, isatty(fd) ? ttyname(fd) : "no");
#ifdef linux
	printf("  dev = %d/%d\n", major(st.st_dev), minor(st.st_dev));
#endif
	printf("  inode = %lu\n", (long) st.st_ino);
}

int
main(void)
{
	printf("fileno(stdout) = %d\n", fileno(stdout));
	printf("fileno(stderr) = %d\n", fileno(stderr));
	printf("=== before dup\n");
	statfd(1);
	statfd(2);

	if (dup2(2, 1) == -1)
		err(1, "dup2");
	printf("=== after dup\n");
	fprintf(stdout, "this is standard output\n");

	statfd(1);
	statfd(2);
}
