/* See LICENSE file for copyright and license details. */

#include "../components_config.h"
#include "../lib/util.h"

#ifdef __linux__
#	include <err.h>
#	include <fcntl.h>
#	include <stdint.h>
#	include <stdio.h>

#	include "../aslstatus.h"
#	define ENTROPY_AVAIL "/proc/sys/kernel/random/entropy_avail"

void
entropy(char*	   out,
	const char __unused* _a,
	uint32_t __unused    _i,
	static_data_t*	     static_data)
{
	ssize_t readed;
	int*	fd = static_data->data;
	char	buf[9 /* len(str(uint32_t)) */ + 1];

	if (!static_data->cleanup) static_data->cleanup = fd_cleanup;

	if (*fd > 0) {
		if (!fd_rewind(*fd)) ERRRET(out);
	} else {
		if ((*fd = open(ENTROPY_AVAIL, O_RDONLY | O_CLOEXEC)) == -1) {
			warn("open(%s)", ENTROPY_AVAIL);
			ERRRET(out);
		}
	}

	if (!eread_ret(readed, *fd, WITH_LEN(buf))) ERRRET(out);

	buf[--readed /* '\n' at the end */] = '\0';

	bprintf(out, "%s", buf);
}

#else
#	include <string.h>
#	ifndef ENTROPY_INFINITY
#		define ENTROPY_INFINITY "\xe2\x88\x9e"
#	endif

void
entropy(char *	   out,
	const char __unused *_a,
	uint32_t __unused    _i,
	void __unused *_p)
{
	bprintf(out, "%s", ENTROPY_INFINITY);
}

#endif
