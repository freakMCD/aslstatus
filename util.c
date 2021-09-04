#include <err.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdint.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>

#include "util.h"

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
	if (evsnprintf(buf, BUFF_SZ, fmt, ap) < 0) buf[0] = '\0';
	va_end(ap);
}

int
pscanf(const char *path, const char *fmt, ...)
{ /* path scansf */
	int	n;
	FILE *	fp;
	va_list ap;

	if (!(fp = fopen(path, "r"))) {
		warn("fopen '%s'", path);
		return -1;
	}

	va_start(ap, fmt);
	n = vfscanf(fp, fmt, ap);
	va_end(ap);

	fclose(fp);
	return (n == EOF) ? -1 : n;
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
fmt_human(char *out, uintmax_t num, unsigned short int base)
{
	double		   scaled;
	unsigned int	   i;
	const char **	   prefix;
	const unsigned int prefixlen	 = 9;
	const char *	   prefix_1000[] = { "",  "k", "M", "G", "T",
					     "P", "E", "Z", "Y" };
	const char *	   prefix_1024[] = { "",   "Ki", "Mi", "Gi", "Ti",
					     "Pi", "Ei", "Zi", "Yi" };

	switch (base) {
	case 1000:
		prefix = prefix_1000;
		break;
	case 1024:
		prefix = prefix_1024;
		break;
	default:
		warn("fmt_human: Invalid base");
		ERRRET(out);
	}

	scaled = num;
	for (i = 0; i < prefixlen && scaled >= base; i++) {
		scaled /= base;
	}
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
