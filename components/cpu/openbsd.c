#include <err.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include <sys/param.h>
#include <sys/sched.h>
#include <sys/sysctl.h>

#include "../../util.h"

void
cpu_freq(char *	    out,
	 const char __unused * _a,
	 unsigned int __unused _i,
	 void __unused *_p)
{
	int    freq, mib[2];
	size_t size;

	mib[0] = CTL_HW;
	mib[1] = HW_CPUSPEED;

	size   = sizeof(freq);

	/* in MHz */
	if (sysctl(mib, 2, &freq, &size, NULL, 0) < 0) {
		warn("sysctl 'HW_CPUSPEED'");
		ERRRET(out);
	}

	fmt_human(out, freq * 1E6);
}

void
cpu_perc(char *	    out,
	 const char __unused * _a,
	 unsigned int __unused _i,
	 void *		       static_ptr)
{
	int	   mib[2];
	size_t	   size;
	uintmax_t  sum, b[CPUSTATES];
	uintmax_t *a = static_ptr;

	mib[0]	     = CTL_KERN;
	mib[1]	     = KERN_CPTIME;

	size	     = sizeof(b);

	memcpy(b, a, size);
	if (sysctl(mib, 2, &a, &size, NULL, 0) < 0) {
		warn("sysctl 'KERN_CPTIME'");
		ERRRET(out);
	}
	if (!b[0]) ERRRET(out);

	sum = (a[CP_USER] + a[CP_NICE] + a[CP_SYS] + a[CP_INTR] + a[CP_IDLE])
	    - (b[CP_USER] + b[CP_NICE] + b[CP_SYS] + b[CP_INTR] + b[CP_IDLE]);

	if (!sum) ERRRET(out);

	bprintf(out,
		"%d",
		100
		    * ((a[CP_USER] + a[CP_NICE] + a[CP_SYS] + a[CP_INTR])
		       - (b[CP_USER] + b[CP_NICE] + b[CP_SYS] + b[CP_INTR]))
		    / sum);
}
