#ifndef _THREAD_HELPER_H
#define _THREAD_HELPER_H

#include "os.h"

#if LINUX
#	ifndef _GNU_SOURCE
#		define _GNU_SOURCE /* for pthread_setname_np */
#	endif
#endif
#include <pthread.h>

#if ISBSD
#	include <pthread_np.h>
#endif

#if LINUX
#	define pthread_setname pthread_setname_np
#else
#	define pthread_setname pthread_set_name_np
#endif

#endif /* _THREAD_HELPER_H */
