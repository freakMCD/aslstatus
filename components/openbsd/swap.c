#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include <stdlib.h>
#include <unistd.h>
#include <sys/swap.h>
#include <sys/types.h>

#include "../../lib/util.h"

static inline int
getstats(int *total, int *used)
{
	int		i, nswap, rnswap;
	struct swapent *sep, *fsep;

	if ((nswap = swapctl(SWAP_NSWAP, 0, 0)) < 1) {
		warn("swaptctl 'SWAP_NSWAP'");
		return 1;
	}
	if (!(fsep = sep = calloc(nswap, sizeof(*sep)))) {
		warn("calloc 'nswap'");
		return 1;
	}
	if ((rnswap = swapctl(SWAP_STATS, (void *)sep, nswap)) < 0) {
		warn("swapctl 'SWAP_STATA'");
		return 1;
	}
	if (nswap != rnswap) {
		warnx("getstats: SWAP_STATS != SWAP_NSWAP");
		return 1;
	}

	*total = 0;
	*used  = 0;

	for (i = 0; i < rnswap; i++) {
		*total += sep->se_nblks >> 1;
		*used += sep->se_inuse >> 1;
	}

	free(fsep);

	return 0;
}

void
swap_free(char *     out,
	  const char __unused * _a,
	  unsigned int __unused _i,
	  void __unused *_p)
{
	int used, total;

	if (getstats(&total, &used)) ERRRET(out);

	fmt_human(out, (total - used) * 1024);
}

void
swap_perc(char *     out,
	  const char __unused * _a,
	  unsigned int __unused _i,
	  void __unused *_p)
{
	int used, total;

	if (getstats(&total, &used)) ERRRET(out);

	if (!total) ERRRET(out);

	bprintf(out, "%d", 100 * used / total);
}

void
swap_total(char *     out,
	   const char __unused * _a,
	   unsigned int __unused _i,
	   void __unused *_p)
{
	int used, total;

	if (getstats(&total, &used)) ERRRET(out);

	fmt_human(out, total * 1024);
}

void
swap_used(char *     out,
	  const char __unused * _a,
	  unsigned int __unused _i,
	  void __unused *_p)
{
	int used, total;

	if (getstats(&total, &used)) ERRRET(out);

	fmt_human(out, used * 1024);
}
