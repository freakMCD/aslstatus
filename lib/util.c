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
{
	va_list ap;

	va_start(ap, fmt);
	if (evsnprintf(buf, BUFF_SZ, fmt, ap) < 0) *buf = '\0';
	va_end(ap);
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
_sysfs_fd(const char *func,
	  const char *path,
	  const char *device,
	  const char *property)
{
	uint8_t i;

	int  fds[3]  = { -1, -1, -1 };
	int *path_fd = &fds[0], *device_fd = &fds[1], *property_fd = &fds[2];

	static const int  DIR_FLAGS	= (O_RDONLY | O_CLOEXEC | O_DIRECTORY);
	static const char S_DIR_FLAGS[] = "O_RDONLY | O_CLOEXEC | O_DIRECTORY";

	if ((*path_fd = open(path, DIR_FLAGS)) == -1) {
		warn("%s: open(" QUOTED("%s") ", %s)",
		     func,
		     path,
		     S_DIR_FLAGS);
		goto end;
	}

	if ((*device_fd = openat(*path_fd, device, DIR_FLAGS)) == -1) {
		warn(
		    "%s: openat(fd:%d " QUOTED("%s") ", " QUOTED("%s") ", %s)",
		    func,
		    *path_fd,
		    path,
		    device,
		    S_DIR_FLAGS);
		goto end;
	}

	if (!!property) {
		if ((*property_fd =
			 openat(*device_fd, property, O_RDONLY | O_CLOEXEC))
		    == -1) {
			warn("%s: openat(fd:%d " QUOTED("%s/%s") ", " QUOTED(
				 "%s") ", %s)",
			     func,
			     *device_fd,
			     path,
			     device,
			     property,
			     "O_RDONLY | O_CLOEXEC");
			goto end;
		}
	} else {
		*property_fd = *device_fd;
		*device_fd   = -1;
	}

end:
	for (i = 0; i < (LEN(fds) - 1 /* except last */); i++)
		if (fds[i] >= 0) _eclose(func, fds[i]);

	return *property_fd;
}

uint8_t
_sysfs_fd_or_rewind(const char *func,
		    int *	fd,
		    const char *path,
		    const char *device,
		    const char *property)
{
	if (*fd > 0) return _fd_rewind(func, *fd);

	if ((*fd = _sysfs_fd(func, path, device, property)) != -1) return !0;

	return 0;
}

off_t
_elseek(const char *func, int fd, off_t offset, int whence)
{
	off_t ret;

	if ((ret = lseek(fd, offset, whence)) == (off_t)-1)
		warn("%s: lseek(%d, %ld, %d)", func, fd, offset, whence);

	return ret;
}

uint8_t
_fd_rewind(const char *func, int fd)
{
	return _elseek(func, fd, 0, SEEK_SET) != (off_t)-1;
}

ssize_t
_eread(const char *func, int fd, void *buf, size_t size)
{
	ssize_t ret;

	if ((ret = read(fd, buf, size)) == -1)
		warn("%s: read(%d, %p, %lu)", func, fd, buf, size);

	return ret;
}

uint8_t
_eclose(const char *func, int fd)
{
	uint8_t ret;

	if (!(ret = (close(fd) != -1))) warn("%s: close(%d)", func, fd);

	return ret;
}

uint8_t
_esscanf(const char *func,
	 int	     count,
	 const char *restrict str,
	 const char *restrict fmt,
	 ...)
{
	va_list		  ap;
	int		  err;
	__typeof__(count) match = 0;

	va_start(ap, fmt);
	errno = 0;
	match = vsscanf(str, fmt, ap);
	err   = errno;
	va_end(ap);

	if (match == count)
		return !0;
	else if (err != 0)
		warn("%s: sscanf(%p, " QUOTED("%s") ", ...)", func, str, fmt);
	else
		warnx("%s: sscnaf(fmt: " QUOTED("%s") "): matched %d of %d",
		      func,
		      fmt,
		      match,
		      count);

	return 0;
}

void
fd_cleanup(void *ptr)
{
	int fd = *((int *)ptr);

	if (fd > 0) eclose(fd);
}
