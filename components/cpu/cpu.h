#ifndef CPU_H
#define CPU_H

#include "../../os.h"

#if LINUX
#	include <stdio.h>
#	include <inttypes.h>

/*
 * see /proc/stat in `man 5 proc`
 *
 * guest time is already accounted in usertime,
 * so skip `guest` and `guest_nice`
 */
enum CPU_STATE {
	CPU_STATE_USER	  = 0,
	CPU_STATE_NICE	  = 1,
	CPU_STATE_SYSTEM  = 2,
	CPU_STATE_IDLE	  = 3,
	CPU_STATE_IOWAIT  = 4,
	CPU_STATE_IRQ	  = 5,
	CPU_STATE_SOFTIRQ = 6,
	CPU_STATE_STEAL	  = 7,
	_CPU_STATE_COUNT,
};
struct cpu_data_t {
	FILE*	    fptr;
	long double states[_CPU_STATE_COUNT];
};
#	define CPU_STATIC_SIZE sizeof(struct cpu_data_t)

#else
#	include <sys/param.h> /* CPUSTATES */
#	if OBSD
#		include <inttypes.h>
#		define CPU_STATIC_SIZE sizeof(uintmax[CPUSTATES])
#	else
#		define CPU_STATIC_SIZE sizeof(long[CPUSTATES])
#	endif
#endif

#endif /* CPU_H */
