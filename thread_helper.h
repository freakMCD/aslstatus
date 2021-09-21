#ifndef _THREAD_HELPER_H
#define _THREAD_HELPER_H

#ifndef _GNU_SOURCE
#	define _GNU_SOURCE /* for pthread_*_np */
#endif
#include <pthread.h>

__typeof__(pthread_setname_np)(*pthread_setname) = pthread_setname_np;

#endif /* _THREAD_HELPER_H */
