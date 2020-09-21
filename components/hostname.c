/* See LICENSE file for copyright and license details. */
#include <stdio.h>
#include <unistd.h>

#include "../util.h"

void
hostname(char *out)
{
	if (gethostname(out, BUFF_SZ) < 0) {
		warn("gethostbyname:");
		ERRRET(out);
	}
}
