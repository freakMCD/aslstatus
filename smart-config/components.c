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

/*
 * LOF - Linux || OpenBSD || FreeBSD
 * LB  - Linux || BSD
 */
#if LINUX
#	define LOF_FILE "linux"
#	define LB_FILE	 "linux"
#else
#	define LB_FILE "bsd"
#	if OBSD
#		define LOF_FILE "openbsd"
#	elif FBSD
#		define LOF_FILE "freebsd"
#	endif
#endif

static inline void
_battery(void)
{
	puts("file:battery/" LOF_FILE ".c");
}

static inline void
_netspeed(void)
{
	puts("file:netspeed/" LB_FILE ".c");
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
void netspeed_rx FUNC_ARGS { _netspeed(); }

void netspeed_tx FUNC_ARGS { _netspeed(); }

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
