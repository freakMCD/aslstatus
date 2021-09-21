#include <err.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>

#include <limits.h>
#include <stdint.h>
#include <unistd.h>

#include "battery.h"
#include "../lib/util.h"
#include "../aslstatus.h"

#define MAX_STATE     13
#define STATE_PATTERN "%" STR(MAX_STATE) "[^\n]s"
#define SYS_CLASS     "/sys/class/power_supply"

static uint8_t pick(const char *bat, int *fd, const char **arr, size_t len);
static uint8_t get_state(char state[MAX_STATE], int *fd, const char *bat);
static void    battery_remaining_cleanup(void *ptr);

void
battery_perc(char *	       out,
	     const char *      bat,
	     uint32_t __unused _i,
	     static_data_t *   static_data)
{
	ssize_t readed;
	char	perc[4]; /* len(str(100)) + 1 */
	int *	fd = static_data->data;

	if (!static_data->cleanup) static_data->cleanup = fd_cleanup;

	if (!sysfs_fd_or_rewind(fd, SYS_CLASS, bat, "capacity")) ERRRET(out);

	if (!eread_ret(readed, *fd, WITH_LEN(perc))) ERRRET(out);

	perc[--readed /* '\n' at the end */] = '\0';

	bprintf(out, "%.3s", perc);
}

void
battery_state(char *		out,
	      const char *	bat,
	      uint32_t __unused _i,
	      static_data_t *	static_data)
{
	int *fd = static_data->data;

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

	if (!static_data->cleanup) static_data->cleanup = fd_cleanup;

	if (!get_state(state, fd, bat)) ERRRET(out);

	for (i = 0; i < LEN(map); i++)
		if (!strcmp(map[i].state, state)) break;

	bprintf(out, "%s", (i == LEN(map)) ? BATTERY_UNKNOWN : map[i].symbol);
}

void
battery_remaining(char *	    out,
		  const char *	    bat,
		  uint32_t __unused _i,
		  static_data_t *   static_data)
{
	struct remaining *fds = static_data->data;

	char	  buf[JU_STR_SIZE];
	char	  state[MAX_STATE];
	uintmax_t m, h, charge_now, current_now;

#define RESOLVE(S, F) *F = &S->F
	int RESOLVE(fds, status), RESOLVE(fds, charge), RESOLVE(fds, current);

	double timeleft;

	static const char *charge_arr[] = {
		"charge_now",
		"energy_now",
	};
	static const char *current_arr[] = {
		"current_now",
		"power_now",
	};

	if (!static_data->cleanup)
		static_data->cleanup = battery_remaining_cleanup;

	if (!get_state(state, status, bat)) ERRRET(out);

#define pick_scan(B, C)                                                       \
	(pick(bat, C, C##_arr, LEN(C##_arr)) && eread(*C, WITH_LEN(buf))      \
	 && esscanf(1, buf, "%ju", &C##_now))

	if (!strcmp(state, "Discharging")) {
		do {
			if (pick_scan(buf, charge) && pick_scan(buf, current))
				break;

			ERRRET(out);
		} while (0);

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

	if (*fd > 0) return fd_rewind(*fd);

	if ((bat_fd = sysfs_fd(SYS_CLASS, bat, NULL)) == -1) return 0;

	for (i = 0; i < len; i++) {
		if ((*fd = openat(bat_fd, arr[i], O_RDONLY | O_CLOEXEC))
		    != -1) {
			ret = !0;
			goto end;
		}
	}

end:
	if (bat_fd != -1) eclose(bat_fd);
	return ret;
}

static inline uint8_t
get_state(char state[MAX_STATE], int *fd, const char *bat)
{
	ssize_t readed;

	if (!sysfs_fd_or_rewind(fd, SYS_CLASS, bat, "status")) return 0;
	if (!eread_ret(readed, *fd, state, MAX_STATE)) return !0;

	state[--readed /* '\n' at the end */] = '\0';

	return !0;
}

static inline void
battery_remaining_cleanup(void *ptr)
{
	int fds[] = {
		((struct remaining *)ptr)->status,
		((struct remaining *)ptr)->charge,
		((struct remaining *)ptr)->current,
	};

	for (uint8_t i = 0; i < LEN(fds); i++)
		if (!!fds[i]) eclose(fds[i]);
}
