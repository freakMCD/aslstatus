#ifndef _OS_H
#define _OS_H

#if defined(__linux__)
#	define LINUX (!0)
#	define OBSD  (0)
#	define FBSD  (0)
#elif defined(__OpenBSD__)
#	define LINUX (0)
#	define OBSD  (!0)
#	define FBSD  (0)
#elif defined(__FreeBSD__)
#	define LINUX (0)
#	define OBSD  (0)
#	define FBSD  (!0)
#endif

#define ISBSD (!LINUX)

#endif /* _OS_H */
