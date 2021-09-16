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

#define JU_STR_SIZE                                                           \
	(20 /* max string size size of %ju (uintmax_t) */ + 1 /* '\n' */)

#define __unused	   __attribute__((__unused__))
#define typeof_field(S, F) __typeof__(((S *)0)->F)
#define TWICE(V)	   V, V

#define STRINGIFY_AUX(X) #X
#define STR(X)		 STRINGIFY_AUX(X)

#define STR_SIZE(S)   (LEN(S) - 1) /* minus null byte */
#define WITH_SSIZE(S) S, STR_SIZE(S)

#define LEN(S)	    (sizeof(S) / sizeof *(S))
#define WITH_LEN(S) S, LEN(S)

#define ERRRET(B)                                                             \
	do {                                                                  \
		(B)[0] = '\0';                                                \
		return;                                                       \
	} while (0)

/*
 * warn if function fails
 * COND	condition to test if failed
 * ERR	error block
 * R	return value, if you don't need it pass here `_unused`
 * F	function
 */
#define EFUNC(COND, ERR, R, F, ...)                                           \
	do {                                                                  \
		uintmax_t __unused _unused;                                   \
		if (COND(R = F(__VA_ARGS__))) {                               \
			warn("%s: %s(%s)", __func__, #F, #__VA_ARGS__);       \
			ERR                                                   \
		}                                                             \
	} while (0)

/*
 * seek FD to 0
 * if error: run second arg
 * example:
 *   SEEK_0(fd, { return !0; });
 */
#define SEEK_0(ERR, FD) EFUNC(!!, ERR, _unused, lseek, (FD), 0, SEEK_SET)

/*
 * read(2) wraper to warn on error
 * ERR	error block
 * R	return value (see EFUNC define)
 * ...	args to read
 */
#define EREAD(ERR, R, ...) EFUNC(-1 == (int), ERR, R, read, __VA_ARGS__)

/* same as sysfs_fd, but seek to 0 if already opened */
#define SYSFS_FD_OR_SEEK(ERR, FD, PATH, DEV, PROP)                            \
	do {                                                                  \
		if ((FD) > 0)                                                 \
			SEEK_0(ERR, FD);                                      \
		else if (((FD) = sysfs_fd(PATH, DEV, PROP)) == -1)            \
			ERR                                                   \
	} while (0)

/*
 * ERR	error block
 * C	count of matches
 * F	scanf function
 * ...	args to scanf
 */
#define SCANF(ERR, C, F, ...)                                                 \
	do {                                                                  \
		int _m = errno = 0;                                           \
		if ((_m = F(__VA_ARGS__)) == (C)) {                           \
			break;                                                \
		} else if (errno != 0) {                                      \
			warn("%s: %s", __func__, #F);                         \
			ERR                                                   \
		} else {                                                      \
			warnx("%s: %s: matched %d of %ld",                    \
			      __func__,                                       \
			      #F,                                             \
			      _m,                                             \
			      (size_t)(C));                                   \
			ERR                                                   \
		}                                                             \
	} while (0)

void bprintf(char *, const char *, ...);
int  esnprintf(char *, size_t, const char *, ...);
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
int sysfs_fd(const char *, const char *, const char *);

#endif /* _UTIL_H */
