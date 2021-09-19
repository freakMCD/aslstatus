#include <err.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdint.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>

#include "util.h"
#include "numfmt.h"
#include "../components_config.h"

#ifndef FMT_HUMAN_NUMFMT
#	define FMT_HUMAN_NUMFMT NUMFMT_IEC
#endif

static inline int
evsnprintf(char *str, size_t size, const char *fmt, va_list ap)
{
	int ret;

	ret = vsnprintf(str, size, fmt, ap);

	if (ret < 0) {
		warn("vsnprintf");
		return -1;
	} else if ((size_t)ret >= size) {
		warnx("vsnprintf: Output truncated");
		return -1;
	}

	return ret;
}

void
bprintf(char *buf, const char *fmt, ...)
{ /* buffer printf */
	va_list ap;

	va_start(ap, fmt);
	if (evsnprintf(buf, BUFF_SZ, fmt, ap) < 0) *buf = '\0';
	va_end(ap);
}

int
esnprintf(char *str, size_t size, const char *fmt, ...)
{ /* snprintf with warn about truncating */
	va_list ap;
	int	ret;

	va_start(ap, fmt);
	ret = evsnprintf(str, size, fmt, ap);
	va_end(ap);

	return ret;
}

void
fmt_human(char *out, uintmax_t num)
{
	double	     scaled;
	unsigned int i;
	const char * prefix[] =
#if FMT_HUMAN_NUMFMT == NUMFMT_IEC
	{ "",
	  "Ki",
	  "Mi",
	  "Gi",
	  "Ti",
	  "Pi",
	  "Ei",
	  "Zi",
	  "Yi" }
#elif FMT_HUMAN_NUMFMT == NUMFMT_SI
	{ "",
	  "k",
	  "M",
	  "G",
	  "T",
	  "P",
	  "E",
	  "Z",
	  "Y" }
#else
#	error invalid FMT_HUMAN_NUMFMT
#endif
	;

	scaled = num;
	for (i = 0; i < LEN(prefix) && scaled >= FMT_HUMAN_NUMFMT; i++)
		scaled /= FMT_HUMAN_NUMFMT;

	bprintf(out, "%.1f %s", scaled, prefix[i]);
}

int
sysfs_fd(const char *path, const char *device, const char *property)
{
	uint8_t i;

	int  fds[3];
	int *path_fd = &fds[0], *device_fd = &fds[1], *property_fd = &fds[2];

	static const int DIR_FLAGS = O_RDONLY | O_CLOEXEC | O_DIRECTORY;

	memset(fds, -1, sizeof(fds));

	if ((*path_fd = open(path, DIR_FLAGS)) == -1) {
		warn("%s", path);
		goto end;
	}

	if ((*device_fd = openat(*path_fd, device, DIR_FLAGS)) == -1) {
		warn("%s/%s", path, device);
		goto end;
	}

	if (!!property) {
		if ((*property_fd =
			 openat(*device_fd, property, O_RDONLY | O_CLOEXEC))
		    == -1) {
			warn("%s/%s/%s", path, device, property);
			goto end;
		}
	} else {
		*property_fd = *device_fd;
		*device_fd   = -1;
	}

end:
	for (i = 0; i < (LEN(fds) - 1 /* except last */); i++)
		if (fds[i] >= 0)
			if (close(fds[i]) == -1) warn("%d", fds[i]);

	return *property_fd;
}

void
fd_cleanup(void *ptr)
{
	CCLOSE(*((int *)ptr));
}
