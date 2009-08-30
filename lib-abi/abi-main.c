/*
 * An example on how a change in a lib ABI can screw programs that use the
 * library.
 *
 * The program adds 2 32-bit integers. When lib's ABI changes, the program
 * doesn't know about it and prints an incorrect value. Run like this:
 *
 *   $ cc -shared -o libabi.so libabi-32.c
 *   $ cc -L. -R. -labi abi-main.c
 *   $ ./a.out 7 4
 *   11
 *   $ cc -shared -o libabi.so libabi-64.c
 *   $ ./a.out 7 4
 *   -1077941116
 *
 * (c) jp@devnull.cz
 */

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <err.h>

int32_t abiadd(int32_t a, int32_t b);

int
main(int argc, char **argv)
{
	if (argc != 3)
		errx(1, "usage: a.out <number> <number>");

	printf("%d\n", abiadd(atoi(argv[1]), atoi(argv[2])));

	return (0);
}
