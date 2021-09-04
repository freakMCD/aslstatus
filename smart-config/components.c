#include <stdio.h>

#include "../os.h"
#include "../util.h"

#define FUNC_ARGS                                                             \
	(char __unused* _o,                                                   \
	 const char __unused*  _a,                                            \
	 unsigned int __unused _i,                                            \
	 void __unused* _p)

#define DEF(F)                                                                \
	void F FUNC_ARGS { puts(#F); }

static inline void
_battery(void)
{
	puts(
	    "file:battery/"
#if LINUX
	    "linux"
#elif OBSD
	    "openbsd"
#elif FBSD
	    "freebsd"
#endif
	    ".c");
}

/* battery */
void battery_perc FUNC_ARGS { _battery(); }

void battery_state FUNC_ARGS { _battery(); }

void battery_remaining FUNC_ARGS { _battery(); }

/* bspwm */
DEF(bspwm_ws)

/* cpu */
DEF(cpu_freq)

DEF(cpu_perc)

/* datetime */
DEF(datetime)

/* disk */
DEF(disk_free)

DEF(disk_perc)

DEF(disk_total)

DEF(disk_used)

/* entropy */
DEF(entropy)

/* hostname */
DEF(hostname)

/* ip */
DEF(ipv4)

DEF(ipv6)

/* kernel_release */
DEF(kernel_release)

/* keymap */
DEF(keymap)

/* load_avg */
DEF(load_avg)

/* netspeeds */
DEF(netspeed_rx)

DEF(netspeed_tx)

/* num_files */
DEF(num_files)

/* ram */
DEF(ram_free)

DEF(ram_perc)

DEF(ram_total)

DEF(ram_used)

/* run_command */
DEF(run_command)

/* swap */
DEF(swap_free)

DEF(swap_perc)

DEF(swap_total)

DEF(swap_used)

/* temperature */
DEF(temp)

/* uptime */
DEF(uptime)

/* user */
DEF(gid)

DEF(uid)

DEF(username)

/* volume */
void vol_perc FUNC_ARGS
{
	puts(
	    "file:volume/"
#if defined(USE_ALSA)
	    "alsa"
#elif defined(USE_PULSE)
	    "pulse"
#else
	    "volume"
#endif
	    ".c");
}

/* wifi */
DEF(wifi_perc)

DEF(wifi_essid)
