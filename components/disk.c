/* See LICENSE file for copyright and license details. */
#include <err.h>
#include <stdio.h>
#include <sys/statvfs.h>

#include "../lib/util.h"

void
disk_free(char *out, const char *path, uint32_t __unused _i, void __unused *_p)
{
	struct statvfs fs;

	if (statvfs(path, &fs) < 0) {
		warn("statvfs '%s'", path);
		ERRRET(out);
	}

	fmt_human(out, fs.f_frsize * fs.f_bavail);
}

void
disk_perc(char *out, const char *path, uint32_t __unused _i, void __unused *_p)
{
	struct statvfs fs;

	if (statvfs(path, &fs) < 0) {
		warn("statvfs '%s'", path);
		ERRRET(out);
	}

	bprintf(
	    out,
	    "%" PRIperc,
	    (percent_t)(100
			* (1.0f - ((float)fs.f_bavail / (float)fs.f_blocks))));
}

void
disk_total(char *	     out,
	   const char *	     path,
	   uint32_t __unused _i,
	   void __unused *_p)
{
	struct statvfs fs;

	if (statvfs(path, &fs) < 0) {
		warn("statvfs '%s'", path);
		ERRRET(out);
	}

	fmt_human(out, fs.f_frsize * fs.f_blocks);
}

void
disk_used(char *out, const char *path, uint32_t __unused _i, void __unused *_p)
{
	struct statvfs fs;

	if (statvfs(path, &fs) < 0) {
		warn("statvfs '%s'", path);
		ERRRET(out);
	}

	fmt_human(out, fs.f_frsize * (fs.f_blocks - fs.f_bfree));
}
