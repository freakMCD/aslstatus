/* See LICENSE file for copyright and license details. */
#include <err.h>
#include <stdio.h>
#include <unistd.h>

#include "../lib/util.h"

void
hostname(char *	    out,
	 const char __unused *_a,
	 uint32_t __unused    _i,
	 void __unused *_p)
{
	if (gethostname(out, BUFF_SZ) < 0) {
		warnx("gethostbyname");
		ERRRET(out);
	}
}
