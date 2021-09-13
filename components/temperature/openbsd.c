#include <err.h>
#include <stddef.h>

#include "../../lib/util.h"

#include <stdio.h>
#include <sys/time.h> /* before <sys/sensors.h> for struct timeval */
#include <sys/sysctl.h>
#include <sys/sensors.h>

void
temp(char *	out,
     const char __unused * _a,
     unsigned int __unused _i,
     void __unused *_p)
{
	int	      mib[5];
	size_t	      size;
	struct sensor temp;

	mib[0] = CTL_HW;
	mib[1] = HW_SENSORS;
	mib[2] = 0; /* cpu0 */
	mib[3] = SENSOR_TEMP;
	mib[4] = 0; /* temp0 */

	size   = sizeof(temp);

	if (sysctl(mib, 5, &temp, &size, NULL, 0) < 0) {
		warn("sysctl 'SENSOR_TEMP'");
		ERRRET(out);
	}

	/* kelvin to celsius */
	bprintf(out, "%d", (temp.value - 273150000) / 1E6);
}
