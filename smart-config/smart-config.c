#include <stdio.h>

#include "../lib/util.h"
#include "../aslstatus.h"

#include "../config.h"

int
main(void)
{
	unsigned int i;

	puts("file:../lib/util.c"); /* every component needs util */

	for (i = 0; i < LEN(args); i++)
		args[i].f.func(NULL, NULL, 0, NULL);

	return 0;
}
