#include <err.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <sys/param.h>

#include <sys/sysctl.h>
#include <devstat.h>

#include "../../util.h"

void
cpu_freq(char *	    out,
	 const char __unused * _a,
	 unsigned int __unused _i,
	 void __unused *_p)
{
	int    freq;
	size_t size;

	size = sizeof(freq);
	/* in MHz */
	if (sysctlbyname("hw.clockrate", &freq, &size, NULL, 0) == -1
	    || !size) {
		warn("sysctlbyname 'hw.clockrate'");
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
	size_t size;
	long   sum, b[CPUSTATES];
	long * a = static_ptr;

	size	 = sizeof(b);
	memcpy(b, a, size);
	if (sysctlbyname("kern.cp_time", &a, &size, NULL, 0) == -1 || !size) {
		warn("sysctlbyname 'kern.cp_time'");
		ERRRET(out);
	}
	if (!b[0]) { ERRRET(out); }

	sum = (a[CP_USER] + a[CP_NICE] + a[CP_SYS] + a[CP_INTR] + a[CP_IDLE])
	    - (b[CP_USER] + b[CP_NICE] + b[CP_SYS] + b[CP_INTR] + b[CP_IDLE]);

	if (!sum) { ERRRET(out); }

	bprintf(out,
		"%d",
		100
		    * ((a[CP_USER] + a[CP_NICE] + a[CP_SYS] + a[CP_INTR])
		       - (b[CP_USER] + b[CP_NICE] + b[CP_SYS] + b[CP_INTR]))
		    / sum);
}
