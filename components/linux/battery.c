#include <err.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>

#include <limits.h>
#include <stdint.h>
#include <unistd.h>

#include "../battery.h"
#include "../../lib/util.h"

#define MAX_STATE     13
#define STATE_PATTERN "%" STR(MAX_STATE) "[^\n]s"
#define SYS_CLASS     "/sys/class/power_supply"

static inline uint8_t
pick(const char *bat, int *fd, const char **arr, size_t len);
static inline uint8_t
get_state(char state[MAX_STATE], int *fd, const char *bat);

void
battery_perc(char *		   out,
	     const char *	   bat,
	     unsigned int __unused _i,
	     void *		   static_ptr)
{
	size_t readed;
	char   perc[4]; /* len(str(100)) + 1 */
	int *  fd = static_ptr;

	SYSFS_FD_OR_SEEK({ ERRRET(out); }, *fd, SYS_CLASS, bat, "capacity");

	EREAD({ ERRRET(out); }, readed, *fd, WITH_LEN(perc));

	perc[--readed /* '\n' at the end */] = '\0';

	bprintf(out, "%.3s", perc);
}

void
battery_state(char *		    out,
	      const char *	    bat,
	      unsigned int __unused _i,
	      void *		    static_ptr)
{
	int *fd = static_ptr;

	size_t i;
	char   state[MAX_STATE];
	static const struct {
		const char *state;
		const char *symbol;
	} map[] = {
		{ "Charging", BATTERY_CHARGING },
		{ "Discharging", BATTERY_DISCHARGING },
		{ "Not charging", BATTERY_FULL },
	};

	if (get_state(state, fd, bat)) ERRRET(out);

	for (i = 0; i < LEN(map); i++)
		if (!strcmp(map[i].state, state)) break;

	bprintf(out, "%s", (i == LEN(map)) ? BATTERY_UNKNOWN : map[i].symbol);
}

void
battery_remaining(char *		out,
		  const char *		bat,
		  unsigned int __unused _i,
		  void *		static_ptr)
{
	struct remaining *fds = static_ptr;

	char	  state[MAX_STATE];
	uintmax_t m, h, charge_now, current_now;

	int *status = &fds->status, *charge = &fds->charge,
	    *current = &fds->current;

	double timeleft;

	static const char *charge_arr[] = {
		"charge_now",
		"energy_now",
	};
	static const char *current_arr[] = {
		"current_now",
		"power_now",
	};

	if (get_state(state, status, bat)) ERRRET(out);

#define pick_scan(ERR, C)                                                     \
	do {                                                                  \
		char _buf[JU_STR_SIZE];                                       \
		if (!pick(bat, C, C##_arr, LEN(C##_arr))) ERR;                \
		EREAD(ERR, _unused, *C, WITH_LEN(_buf));                      \
		SCANF(ERR, 1, sscanf, _buf, "%ju", &C##_now);                 \
	} while (0)

	if (!strcmp(state, "Discharging")) {
		pick_scan({ ERRRET(out); }, charge);
		pick_scan({ ERRRET(out); }, current);

		if (!current_now) goto not_discharging;

		timeleft = (double)charge_now / (double)current_now;
		h	 = timeleft;
		m	 = (timeleft - (double)h) * 60;

		bprintf(out, "%juh %jum", h, m);
		return;
	}
not_discharging:
	bprintf(out, "%s", BATTERY_REMAINING_NOT_DISCHARGING);
}

static inline uint8_t
pick(const char *bat, int *fd, const char **arr, size_t len)
{
	__typeof__(len) i;

	uint8_t ret    = 0;
	int	bat_fd = -1;

	if (*fd > 0) {
		SEEK_0({ return 0; }, *fd);
		return !0;
	}

	if ((bat_fd = sysfs_fd(SYS_CLASS, bat, NULL)) == -1) return 0;

	for (i = 0; i < len; i++) {
		if ((*fd = openat(bat_fd, arr[i], O_RDONLY | O_CLOEXEC))
		    != -1) {
			ret = !0;
			goto end;
		}
	}

end:
	if (bat_fd != -1) close(bat_fd);
	return ret;
}

static inline uint8_t
get_state(char state[MAX_STATE], int *fd, const char *bat)
{
	size_t readed;

	SYSFS_FD_OR_SEEK({ return !0; }, *fd, SYS_CLASS, bat, "status");
	EREAD({ return !0; }, readed, *fd, state, MAX_STATE);

	state[--readed /* '\n' at the end */] = '\0';

	return 0;
}
