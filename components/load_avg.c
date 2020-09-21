/* See LICENSE file for copyright and license details. */
#include <stdio.h>
#include <stdlib.h>

#include "../util.h"

void
load_avg(char *out)
{
	double avgs[3];

	if (getloadavg(avgs, 3) < 0) {
		warn("getloadavg: Failed to obtain load average");
		ERRRET(out);
	}

	bprintf(out, "%.2f %.2f %.2f", avgs[0], avgs[1], avgs[2]);
}
