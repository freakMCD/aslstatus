#ifndef _UTIL_H
#define _UTIL_H

#include <err.h>    /* warn */
#include <stdio.h>  /* fseek */
#include <stdint.h> /* uintmax_t, uint8_t */
#include <stdarg.h> /* ... */

#define BUFF_SZ 96
/*
 * buffer size for one commponent
 * if you are using `wifi.c` it must be at least 78 bytes
 */

#define LEN(S) (sizeof(S) / sizeof *(S))
#define ERRRET(B)                                                             \
	do {                                                                  \
		(B)[0] = '\0';                                                \
		return;                                                       \
	} while (0)

/*
 * seek F to 0
 * if error: run second arg
 * example:
 *   SEEK_0(fptr, { return !0; });
 */
#define SEEK_0(F, ...)                                                        \
	do {                                                                  \
		if (!!fseek((F), 0, SEEK_SET)) {                              \
			warn("seek");                                         \
			__VA_ARGS__                                           \
		}                                                             \
	} while (0)

#define __unused	   __attribute__((__unused__))
#define typeof_field(S, F) __typeof__(((S *)0)->F)
#define TWICE(V)	   V, V

#define STRINGIFY_AUX(X) #X
#define STR(X)		 STRINGIFY_AUX(X)

void	bprintf(char *, const char *, ...);
int	pscanf(const char *, const char *, ...);
int	esnprintf(char *, size_t, const char *, ...);
void	fmt_human(char *, uintmax_t, unsigned short int);

/* 
 * get fd of sysfs file
 * example:
 *   sysfs_fd("/sys/class/power_supply", "BAT0", "capacity")
 *
 * if last arg is NULL, then return fd of directory
 *   sysfs_fd("/sys/class/power_supply", "BAT0", NULL)
 */
int	sysfs_fd(const char *, const char *, const char *);

/*
 * if fptr already opened then seek it to begin
 * otherwise open fptr same as sysfs_fd, but all args must be not NULL
 */
uint8_t sysfs_fptr(FILE **fptr, const char *, const char *, const char *);

#endif /* _UTIL_H */
