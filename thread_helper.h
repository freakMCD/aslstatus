#ifndef _THREAD_HELPER_H
#define _THREAD_HELPER_H

#ifndef _GNU_SOURCE
#	define _GNU_SOURCE /* for pthread_*_np */
#endif
#include <pthread.h>

#define pthread_setname(...) pthread_setname_np(__VA_ARGS__)

#endif /* _THREAD_HELPER_H */
