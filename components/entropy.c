/* See LICENSE file for copyright and license details. */

#include "../os.h"

#if LINUX
#	include <err.h>
#	include <stdint.h>
#	include <stdio.h>

#	include "../util.h"
#	define ENTROPY_AVAIL "/proc/sys/kernel/random/entropy_avail"

void
entropy(char*	   out,
	const char __unused*  _a,
	unsigned int __unused _i,
	void*		      static_ptr)
{
	FILE**	  avail_fptr = static_ptr;
	uintmax_t num;

	if (!*avail_fptr) {
		if (!(*avail_fptr = fopen(ENTROPY_AVAIL, "r"))) {
			warn("fopen(%s)", ENTROPY_AVAIL);
			ERRRET(out);
		}
	} else {
		fseek(*avail_fptr, 0, SEEK_SET);
	}

	if (fscanf(*avail_fptr, "%ju", &num) == EOF) ERRRET(out);

	bprintf(out, "%ju", num);
}

#else
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
