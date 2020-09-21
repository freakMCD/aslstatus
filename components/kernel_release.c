/* See LICENSE file for copyright and license details. */
#include <sys/utsname.h>
#include <stdio.h>

#include "../util.h"

void
kernel_release(char *out)
{
	struct utsname udata;

	if (uname(&udata) < 0) {
		warn("uname:");
		ERRRET(out);
	}

	bprintf(out, "%s", udata.release);
}
