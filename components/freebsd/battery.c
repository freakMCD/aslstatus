#include <err.h>
#include <stdio.h>
#include <string.h>
#include <sys/sysctl.h>

#include "../battery.h"
#include "../../lib/util.h"

void
battery_perc(char *out, const char *unused)
{
	int    cap;
	size_t len;

	len = sizeof(cap);
	if (sysctlbyname("hw.acpi.battery.life", &cap, &len, NULL, 0) == -1
	    || !len)
		ERRRET(out);

	bprintf(out, "%d", cap);
}

void
battery_state(char *	 out,
	      const char __unused * _a,
	      unsigned int __unused _i,
	      void __unused *_p)
{
	int    state;
	size_t len;

	len = sizeof(state);
	if (sysctlbyname("hw.acpi.battery.state", &state, &len, NULL, 0) == -1
	    || !len)
		ERRRET(out);

	switch (state) {
	case 0:
	case 2:
		bprintf(out, BATTERY_CHARGING);
	case 1:
		bprintf(out, BATTERY_DISCHARGING);
	default:
		bprintf(out, BATTERY_UNKNOWN);
	}
}

void
battery_remaining(char *     out,
		  const char __unused * f,
		  unsigned int __unused i,
		  void __unused *p)
{
	int    rem;
	size_t len;

	len = sizeof(rem);
	if (sysctlbyname("hw.acpi.battery.time", &rem, &len, NULL, 0) == -1
	    || !len || rem == -1)
		ERRRET(out);

	bprintf(out, "%uh %02um", rem / 60, rem % 60);
}
