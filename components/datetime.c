/* See LICENSE file for copyright and license details. */
#include <err.h>
#include <time.h>
#include <stdio.h>

#include "../lib/util.h"

void
datetime(char *		       out,
	 const char *	       fmt,
	 unsigned int __unused _i,
	 void __unused *_p)
{
	time_t t;

	t = time(NULL);
	if (!strftime(out, BUFF_SZ, fmt, localtime(&t))) {
		warnx("strftime: Result string exceeds buffer size");
		ERRRET(out);
	}
}
