#ifndef _UTIL_H
#define _UTIL_H

#include <err.h>   /* warn */
#include <stdio.h> /* fseek */
#include <errno.h>
#include <unistd.h> /* lseek */
#include <stdint.h> /* uintmax_t, uint8_t */
#include <stdarg.h> /* ... */

#define BUFF_SZ 96
/*
 * buffer size for one commponent
 * if you are using `wifi.c` it must be at least 78 bytes
 */

#define INT_STR_SIZE                                                          \
	(10 /* max string size size of int32_t (unsigned and signed) */       \
	 + 1 /* '\n' */)
#define JU_STR_SIZE                                                           \
	(20 /* max string size size of %ju (uintmax_t) */ + 1 /* '\n' */)

#define __unused	   __attribute__((__unused__))
#define typeof_field(S, F) __typeof__(((S *)0)->F)
#define TWICE(V)	   V, V

#define STRINGIFY_AUX(X) #X
#define STR(X)		 STRINGIFY_AUX(X)

#define STR_SIZE(S)   (LEN(S) - 1) /* minus null byte */
#define WITH_SSIZE(S) S, STR_SIZE(S)

#define QUOTED(S) "\"" S "\""

#define LEN(S)	    (sizeof(S) / sizeof *(S))
#define WITH_LEN(S) S, LEN(S)

#define MS2S(MS)  ((MS) / 1000)		 /* milliseconds to seconds */
#define MS2NS(MS) (((MS) % 1000) * 1E6)	 /* milliseconds to nanoseconds */
#define MS2US(MS) (((MS) % 1000) * 1000) /* milliseconds to microseconds */

#define ERRRET(B)                                                             \
	do {                                                                  \
		*(B) = '\0';                                                  \
		return;                                                       \
	} while (0)

/* buffer printf */
void bprintf(char *, const char *, ...);

void fmt_human(char *, uintmax_t);

/*
 * get fd of sysfs file
 * example:
 *   sysfs_fd("/sys/class/power_supply", "BAT0", "capacity")
 *
 * if last arg is NULL,
 * then return fd of directory (/sys/class/power_supply/BAT0/)
 *   sysfs_fd("/sys/class/power_supply", "BAT0", NULL)
 */
#define sysfs_fd(path, device, property)                                      \
	_sysfs_fd(__func__, (path), (device), (property))
int _sysfs_fd(const char *func,
	      const char *path,
	      const char *device,
	      const char *property);

/* same as sysfs_fd, but rewind if already opened */
#define sysfs_fd_or_rewind(fd_ptr, path, device, property)                    \
	_sysfs_fd_or_rewind(__func__, (fd_ptr), (path), (device), (property))
uint8_t _sysfs_fd_or_rewind(const char *func,
			    int *	fd,
			    const char *path,
			    const char *device,
			    const char *property);

/*
 * if function has prefix `e`: then it will print warning on error
 * if return type is uint8_t: then 1 on success 0 on error,
 *   otherwise return original return value from function without `e` prefix
 */

#define elseek(fd, offset, whence) _elseek(__func__, fd, offset, whence)
off_t _elseek(const char *func, int fd, off_t offset, int whence);

#define fd_rewind(fd) _fd_rewind(__func__, (fd))
uint8_t _fd_rewind(const char *func, int fd);

#define eread_ret(ret, fd, ...) _eread_macro(ret =, fd, __VA_ARGS__)
#define eread(fd, ...)		_eread_macro(, fd, __VA_ARGS__)
#define _eread_macro(ret, fd, ...)                                            \
	((ret _eread(__func__, (fd), __VA_ARGS__)) != -1)

ssize_t _eread(const char *func, int fd, void *buf, size_t size);

#define eclose(fd) _eclose(__func__, (fd))
uint8_t _eclose(const char *func, int fd);

#define esscanf(count, str, fmt, ...)                                         \
	_esscanf(__func__, (count), (str), (fmt), __VA_ARGS__)
uint8_t _esscanf(const char *func,
		 int	     count,
		 const char *restrict str,
		 const char *restrict fmt,
		 ...);

void fd_cleanup(void *ptr);

#endif /* _UTIL_H */
