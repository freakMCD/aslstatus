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
entropy(char*		     out,
	const char __unused* _a,
	uint32_t __unused    _i,
	static_data_t*	     static_data)
{
	size_t readed;
	int*   fd = static_data->data;
	char   buf[INT_STR_SIZE];

	/* if `static_data` contain only `fd`
	 * then you can use `fd_cleanup` function from util.h */
	if (!static_data->cleanup) static_data->cleanup = fd_cleanup;

	if (*fd > 0) {
		/* if file already opened then rewind */
		if (!fd_rewind(*fd)) ERRRET(out);
	} else {
		/* open file */
		if (!eopen(*fd, ENTROPY_AVAIL, O_RDONLY | O_CLOEXEC))
			ERRRET(out);
	}

	/* read from file and save size of readed data */
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
entropy(char		     *out,
	const char __unused *_a,
	uint32_t __unused    _i,
	void __unused	      *_p)
{
	bprintf(out, "%s", ENTROPY_INFINITY);
}

#endif
