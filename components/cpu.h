#ifndef CPU_H
#define CPU_H

#include <stdio.h>
#include <inttypes.h>

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
	int	  fd;
	uintmax_t states[_CPU_STATE_COUNT];
};

#endif /* CPU_H */
