#ifndef _ASLSTATUS_H
#define _ASLSTATUS_H

#include <stdio.h>    /* FILE */
#include <pthread.h>  /* PTHREAD_MUTEX_INITIALIZER */
#include <inttypes.h> /* uintmax_t in cpu_perc */

#if USE_X
#	include <xcb/xcb.h>
#endif

#include "os.h"
#include "util.h"

#define FUNC_ARGS (char *, const char *, unsigned int, void *)

#define END                                                                   \
	{                                                                     \
		.data = { 0 }, .mutex = PTHREAD_MUTEX_INITIALIZER             \
	}

#define _FILE_ON_LINUX (LINUX * sizeof(FILE *))
/* if linux: sizeof(FILE *); otherwise: 0 */

typedef void(*func_t) FUNC_ARGS;

typedef struct func_data_t {
	func_t	     func;
	const char   name[16];
	unsigned int static_size;
} func_data_t;

struct segment_t {
	char		data[BUFF_SZ];
	pthread_mutex_t mutex;
};

struct arg_t {
	const func_data_t  f;
	const char *	   fmt;
	const char *	   args;
	const unsigned int interval;
	struct segment_t   segment;
};

/* clang-format off */

/* battery */
void battery_perc FUNC_ARGS;
#define battery_perc {battery_perc, "batt_percentage", _FILE_ON_LINUX}

void battery_state FUNC_ARGS;
#define battery_state {battery_state, "batt_state", _FILE_ON_LINUX}

void battery_remaining FUNC_ARGS;
#define battery_remaining \
	{battery_remaining, "batt_remaining", _FILE_ON_LINUX * 3}


#if USE_X
/* bspwm */
void bspwm_ws FUNC_ARGS;
#define bspwm_ws {bspwm_ws, "bspwm", sizeof(int)}
#endif


/* cpu */
#include "components/cpu/cpu.h"

void cpu_freq FUNC_ARGS;
#define cpu_freq {cpu_freq, "cpu_freq", _FILE_ON_LINUX}

void cpu_perc FUNC_ARGS;
#define cpu_perc {cpu_perc, "cpu_percentage", CPU_STATIC_SIZE}

/* datetime */
void datetime FUNC_ARGS;
#define datetime {datetime, "datetime", 0}


/* disk */
void disk_free FUNC_ARGS;
#define disk_free {disk_free, "disk_free", 0}

void disk_perc FUNC_ARGS;
#define disk_perc {disk_perc, "disk_percentage", 0}

void disk_total FUNC_ARGS;
#define disk_total {disk_total, "disk_total", 0}

void disk_used FUNC_ARGS;
#define disk_used {disk_used, "disk_used", 0}


/* entropy */
void entropy FUNC_ARGS;
#define entropy {entropy, "entropy", _FILE_ON_LINUX}


/* hostname */
void hostname FUNC_ARGS;
#define hostname {hostname, "hostname", 0}


/* ip */
void ipv4 FUNC_ARGS;
#define ipv4 {ipv4, "ipv4", 0}

void ipv6 FUNC_ARGS;
#define ipv6 {ipv6, "ipv6", 0}


/* kernel_release */
void kernel_release FUNC_ARGS;
#define kernel_release {kernel_release, "kernel_release", 0}


#if USE_X && USE_XKB
/* keymap */
void keymap FUNC_ARGS;
#define keymap {keymap, "keymap", sizeof(char)}
#endif


/* load_avg */
void load_avg FUNC_ARGS;
#define load_avg {load_avg, "load_avg", 0}


/* netspeeds */
#include "components/netspeed/netspeed.h"

void netspeed_rx FUNC_ARGS;
#define netspeed_rx {netspeed_rx, "netspeed_rx", NETSPEED_STATIC_SIZE}

void netspeed_tx FUNC_ARGS;
#define netspeed_tx {netspeed_tx, "netspeed_tx", NETSPEED_STATIC_SIZE}


/* num_files */
void num_files FUNC_ARGS;
#define num_files {num_files, "num_files", 0}


/* ram */
void ram_free FUNC_ARGS;
#define ram_free {ram_free, "ram_free", 0}

void ram_perc FUNC_ARGS;
#define ram_perc {ram_perc, "ram_percentage", 0}

void ram_total FUNC_ARGS;
#define ram_total {ram_total, "ram_total", 0}

void ram_used FUNC_ARGS;
#define ram_used {ram_used, "ram_used", 0}


/* run_command */
void run_command FUNC_ARGS;
#define run_command {run_command, "cmd", 0}
/*
 * `cmd` thread name hardcodet to be used to run shell commands
 */


/* swap */
void swap_free FUNC_ARGS;
#define swap_free {swap_free, "swap_free", 0}

void swap_perc FUNC_ARGS;
#define swap_perc {swap_perc, "swap_percentage", 0}

void swap_total FUNC_ARGS;
#define swap_total {swap_total, "swap_total", 0}

void swap_used FUNC_ARGS;
#define swap_used {swap_used, "swap_used", 0}


/* temperature */
void temp FUNC_ARGS;
#define temp {temp, "temperature", _FILE_ON_LINUX}


/* uptime */
void uptime FUNC_ARGS;
#define uptime {uptime, "uptime", 0}


/* user */
void gid FUNC_ARGS;
#define gid {gid, "gid", 0}

void uid FUNC_ARGS;
#define uid {uid, "uid", 0}

void username FUNC_ARGS;
#define username {username, "username", 0}


/* volume */
#include "components/volume/volume.h"
void vol_perc FUNC_ARGS;
#define vol_perc {vol_perc, "volume", VOLUME_STATIC_SIZE}


/* wifi */
void wifi_perc FUNC_ARGS;
#define wifi_perc {wifi_perc, "wifi_percentage", 0}

void wifi_essid FUNC_ARGS;
#define wifi_essid {wifi_essid, "wifi_essid", 0}

/* clang-format on */

#endif /* _ASLSTATUS_H */
