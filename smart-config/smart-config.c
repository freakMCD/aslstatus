#include <stdio.h>

#include "../lib/util.h"

#define ASLSTATUS_H_NEED_COMP
#include "../aslstatus.h"

#include "../config.h"

int
main(void)
{
	uint8_t i;

	puts("file:../lib/util.c"); /* every component needs util */

	for (i = 0; i < LEN(args); i++)
		args[i].f.func(NULL, NULL, 0, NULL);

	return 0;
}
