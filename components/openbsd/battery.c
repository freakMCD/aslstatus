#include <err.h>
#include <stdio.h>
#include <string.h>

#include <fcntl.h>
#include <machine/apmvar.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include "../battery.h"
#include "../../lib/util.h"

static inline int
load_apm_power_info(struct apm_power_info *apm_info)
{
	int fd;

	fd = open("/dev/apm", O_RDONLY);
	if (fd < 0) {
		warn("open '/dev/apm'");
		return 0;
	}

	memset(apm_info, 0, sizeof(struct apm_power_info));
	if (ioctl(fd, APM_IOC_GETPOWER, apm_info) < 0) {
		warn("ioctl 'APM_IOC_GETPOWER'");
		close(fd);
		return 0;
	}
	return close(fd), 1;
}

void
battery_perc(char *	out,
	     const char __unused * _a,
	     unsigned int __unused _i,
	     void __unused *_p)
{
	struct apm_power_info apm_info;

	if (load_apm_power_info(&apm_info)) {
		bprintf(out, "%d", apm_info.battery_life);
	}

	ERRRET(out);
}

void
battery_state(char *	 out,
	      const char __unused * _a,
	      unsigned int __unused _i,
	      void __unused *_p)
{
	struct {
		unsigned int state;
		char *	     symbol;
	} map[] = {
		{ APM_AC_ON, BATTERY_CHARGING },
		{ APM_AC_OFF, BATTERY_DISCHARGING },
	};

	size_t i;

	struct apm_power_info apm_info;

	if (!load_apm_power_info(&apm_info)) ERRRET(out);

	for (i = 0; i < LEN(map); i++) {
		if (map[i].state == apm_info.ac_state) break;
	}

	bprintf(out, (i == LEN(map)) ? BATTERY_UNKNOWN : map[i].symbol);
}

void
battery_remaining(char *     out,
		  const char __unused * _a,
		  unsigned int __unused _i,
		  void __unused *_p)
{
	struct apm_power_info apm_info;

	if (load_apm_power_info(&apm_info))
		if (apm_info.ac_state != APM_AC_ON) {
			bprintf(out,
				"%uh %02um",
				apm_info.minutes_left / 60,
				apm_info.minutes_left % 60);
			return;
		}
	bprintf(out, "%s", BATTERY_REMAINING_NOT_DISCHARGING);
}
