#include <stdio.h>

#include "../os.h"
#include "../lib/util.h"

#define FUNC_ARGS                                                             \
	(char __unused* _o,                                                   \
	 const char __unused* _a,                                             \
	 uint32_t __unused    _i,                                             \
	 void __unused* _p)

#define TRAILING_SEMICOLON(...) struct trailing_semicolon

#define DEF(F)                                                                \
	void F FUNC_ARGS { puts(#F); }                                        \
	TRAILING_SEMICOLON()

#define COMP_F(C, O) puts("file:" O "/" C ".c")

#define COMMON_FILE(F, O)                                                     \
	static inline void F(void) { COMP_F(#F, O); }                         \
	TRAILING_SEMICOLON()

/*
 * LOF - Linux || OpenBSD || FreeBSD
 * LB  - Linux || BSD
 */
#if LINUX
#	define LOF_FILE "linux"
#	define LB_FILE	 "linux"
#	define MEMINFO	 puts("file:../lib/meminfo.c")
#else
#	define LB_FILE "bsd"
#	if OBSD
#		define LOF_FILE "openbsd"
#	elif FBSD
#		define LOF_FILE "freebsd"
#	endif
#	define MEMINFO NULL
#endif

#if defined(USE_ALSA)
#	define VOLUME "alsa"
#elif defined(USE_PULSE)
#	define VOLUME "pulse"
#else
#	define VOLUME "volume"
#endif

COMMON_FILE(battery, LOF_FILE);

COMMON_FILE(cpu, LOF_FILE);

COMMON_FILE(netspeed, LB_FILE);

COMMON_FILE(ram, LOF_FILE);

COMMON_FILE(swap, LOF_FILE);

COMMON_FILE(wifi, LOF_FILE);

/* battery */
void battery_perc FUNC_ARGS { battery(); }

void battery_state FUNC_ARGS { battery(); }

void battery_remaining FUNC_ARGS { battery(); }

/* brightness */
DEF(brightness);

/* bspwm */
DEF(bspwm_ws);

/* cpu */
void cpu_freq FUNC_ARGS { cpu(); }

void cpu_perc FUNC_ARGS { cpu(); }

/* datetime */
DEF(datetime);

/* disk */
DEF(disk_free);

DEF(disk_perc);

DEF(disk_total);

DEF(disk_used);

/* entropy */
DEF(entropy);

/* hostname */
DEF(hostname);

/* ip */
DEF(ipv4);

DEF(ipv6);

/* kernel_release */
DEF(kernel_release);

/* keymap */
DEF(keymap);

/* load_avg */
DEF(load_avg);

/* netspeeds */
void netspeed_rx FUNC_ARGS { netspeed(); }

void netspeed_tx FUNC_ARGS { netspeed(); }

/* num_files */
DEF(num_files);

/* ram */
void ram_free FUNC_ARGS
{
	ram();
	MEMINFO;
}

void ram_perc FUNC_ARGS
{
	ram();
	MEMINFO;
}

void ram_total FUNC_ARGS
{
	ram();
	MEMINFO;
}

void ram_used FUNC_ARGS
{
	ram();
	MEMINFO;
}

/* run_command */
DEF(run_command);

/* swap */
void swap_free FUNC_ARGS
{
	swap();
	MEMINFO;
}

void swap_perc FUNC_ARGS
{
	swap();
	MEMINFO;
}

void swap_total FUNC_ARGS
{
	swap();
	MEMINFO;
}

void swap_used FUNC_ARGS
{
	swap();
	MEMINFO;
}

/* temperature */
void temp FUNC_ARGS { COMP_F("temperature", LOF_FILE); }

/* uptime */
DEF(uptime);

/* user */
DEF(gid);

DEF(uid);

DEF(username);

/* volume */
void vol_perc FUNC_ARGS { COMP_F(VOLUME, "volume"); }

/* wifi */
void wifi_perc FUNC_ARGS { wifi(); }

void wifi_essid FUNC_ARGS { wifi(); }
