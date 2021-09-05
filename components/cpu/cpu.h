#ifndef CPU_H
#define CPU_H

#include "../../os.h"

#if LINUX
#	include <stdio.h>
#	include <inttypes.h>
struct cpu_data_t {
	FILE*	  fptr;
	uintmax_t states[7];
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
