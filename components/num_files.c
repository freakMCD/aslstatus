/* See LICENSE file for copyright and license details. */
#include <err.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <inttypes.h>

#include "../lib/util.h"

void
num_files(char *out, const char *path, uint32_t __unused _i, void __unused *_p)
{
	DIR *  fd;
	size_t num;

	struct dirent *dp;

	if (!(fd = opendir(path))) {
		warn("opendir '%s'", path);
		ERRRET(out);
	}

	num = 0;
	while ((dp = readdir(fd))) {
		if (!strcmp(dp->d_name, ".") || !strcmp(dp->d_name, ".."))
			continue; /* skip self and parent */
		num++;
	}

	closedir(fd);
	bprintf(out, "%" PRIu64, num);
}
