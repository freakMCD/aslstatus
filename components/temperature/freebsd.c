#include <err.h>
#include <stddef.h>

#include "../../lib/util.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/sysctl.h>

void
temp(char *out, const char *zone, unsigned int __unused _i, void __unused *_p)
{
	int    temp;
	size_t len;
	char   buf[256];

	len = sizeof(temp);
	snprintf(buf, sizeof(buf), "hw.acpi.thermal.%s.temperature", zone);
	if (sysctlbyname(buf, &temp, &len, NULL, 0) == -1 || !len) ERRRET(out);

	/* kelvin to decimal celcius */
	bprintf(out, "%d.%d", (temp - 2731) / 10, abs((temp - 2731) % 10));
}
