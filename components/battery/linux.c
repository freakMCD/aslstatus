#include <err.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>

#include <limits.h>
#include <stdint.h>
#include <unistd.h>

#include "icons.h"
#include "../../util.h"

#define MAX_STATE     13
#define STATE_PATTERN "%" STR(MAX_STATE) "[^\n]s"
#define SYS_CLASS     "/sys/class/power_supply"

#define SEEK_0(F, ...)                                                        \
	do {                                                                  \
		if (!!fseek((F), 0, SEEK_SET)) {                              \
			warn("seek");                                         \
			__VA_ARGS__                                           \
		}                                                             \
	} while (0)

static inline uint8_t
pick(const char *bat, FILE **fptr, const char **arr, size_t len);
static inline uint8_t
get_fptr(FILE **fptr, const char *bat, const char *property);
static inline uint8_t
get_state(char state[MAX_STATE], FILE **fptr, const char *bat);

void
battery_perc(char *		   out,
	     const char *	   bat,
	     unsigned int __unused _i,
	     void *		   static_ptr)
{
	uint8_t perc;
	FILE ** perc_fptr = static_ptr;

	if (get_fptr(perc_fptr, bat, "capacity")) ERRRET(out);

	if (fscanf(*perc_fptr, "%hhu", &perc) == EOF) ERRRET(out);

	bprintf(out, "%hhu", perc);
}

void
battery_state(char *		    out,
	      const char *	    bat,
	      unsigned int __unused _i,
	      void *		    static_ptr)
{
	FILE **state_fptr = static_ptr;

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

	if (get_state(state, state_fptr, bat)) ERRRET(out);

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
	char	  state[MAX_STATE];
	uintmax_t m, h, charge_now, current_now;

	FILE **files  = static_ptr;

	FILE **status = &files[0], **charge = &files[1], **current = &files[2];

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

#define pick_scan(C)                                                          \
	(pick(bat, C, C##_arr, LEN(C##_arr))                                  \
	 || fscanf(*C, "%ju", &C##_now) == EOF)

	if (!strcmp(state, "Discharging")) {
		if (pick_scan(charge) || pick_scan(current)) ERRRET(out);

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
pick(const char *bat, FILE **fptr, const char **arr, size_t len)
{
	__typeof__(len) i;

	uint8_t ret    = !0;
	int	bat_fd = -1, fd;

	if (!!*fptr) {
		SEEK_0(*fptr, { return !0; });
		return 0;
	}

	if ((bat_fd = sysfs_fd(SYS_CLASS, bat, NULL)) == -1) return !0;

	for (i = 0; i < len; i++) {
		if ((fd = openat(bat_fd, arr[i], O_RDONLY | O_CLOEXEC))
		    != -1) {
			if (!(*fptr = fdopen(fd, "r"))) {
				warn("fdopen(%d, r)", fd);
				goto end;
			}
			ret = 0;
			goto end;
		}
	}

end:
	if (bat_fd != -1) close(bat_fd);
	return ret;
}

static inline uint8_t
get_fptr(FILE **fptr, const char *bat, const char *property)
{
	int fd;
	if (!*fptr) {
		if ((fd = sysfs_fd(SYS_CLASS, bat, property)) == -1) return !0;

		if (!(*fptr = fdopen(fd, "r"))) {
			warn("fdopen(%d, r)", fd);
			return !0;
		}
	} else {
		SEEK_0(*fptr, { return !0; });
	}

	return 0;
}

static inline uint8_t
get_state(char state[MAX_STATE], FILE **fptr, const char *bat)
{
	if (get_fptr(fptr, bat, "status")) return !0;

	return fscanf(*fptr, STATE_PATTERN, state) == EOF;
}
