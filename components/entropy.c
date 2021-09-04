/* See LICENSE file for copyright and license details. */

#include "../os.h"

#if LINUX
#	include <stdint.h>
#	include <stdio.h>

#	include "../util.h"

void
entropy(char *	   out,
	const char __unused * _a,
	unsigned int __unused _i,
	void __unused *_p)
{
	uintmax_t num;

	if (pscanf("/proc/sys/kernel/random/entropy_avail", "%ju", &num) != 1)
		ERRRET(out);

	bprintf(out, "%ju", num);
}

#elif IF_BSD
#	include <string.h>
#	ifndef ENTROPY_INFINITY
#		define ENTROPY_INFINITY "\xe2\x88\x9e"
#	endif

void
entropy(char *	   out,
	const char __unused * _a,
	unsigned int __unused _i,
	void __unused *_p)
{
	bprintf(out, "%s", ENTROPY_INFINITY);
}

#endif
