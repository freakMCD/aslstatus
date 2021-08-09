/*
 * function            description                     argument (example)
 *
 * battery_perc        battery percentage              battery name (BAT0)
 *                                                     NULL on OpenBSD/FreeBSD
 * battery_state       battery charging state          battery name (BAT0)
 *                                                     NULL on OpenBSD/FreeBSD
 * battery_remaining   battery remaining HH:MM         battery name (BAT0)
 *                                                     NULL on OpenBSD/FreeBSD
 * bspwm_ws            bspwm workspaces for lemonbar   NULL
 * cpu_perc            cpu usage in percent            NULL
 * cpu_freq            cpu frequency in MHz            NULL
 * datetime            date and time                   format string (%F %T)
 * disk_free           free disk space in GB           mountpoint path (/)
 * disk_perc           disk usage in percent           mountpoint path (/)
 * disk_total          total disk space in GB          mountpoint path (/")
 * disk_used           used disk space in GB           mountpoint path (/)
 * entropy             available entropy               NULL
 * gid                 GID of current user             NULL
 * hostname            hostname                        NULL
 * ipv4                IPv4 address                    interface name (eth0)
 * ipv6                IPv6 address                    interface name (eth0)
 * kernel_release      `uname -r`                      NULL
 * keymap              layout of current keymap        NULL
 * load_avg            load average                    NULL
 * netspeed_rx         receive network speed           interface name (wlan0)
 * netspeed_tx         transfer network speed          interface name (wlan0)
 * num_files           number of files in a directory  path
 *                                                     (/home/foo/Inbox/cur)
 * ram_free            free memory in GB               NULL
 * ram_perc            memory usage in percent         NULL
 * ram_total           total memory size in GB         NULL
 * ram_used            used memory in GB               NULL
 * run_command         custom shell command            command (echo foo)
 * swap_free           free swap in GB                 NULL
 * swap_perc           swap usage in percent           NULL
 * swap_total          total swap size in GB           NULL
 * swap_used           used swap in GB                 NULL
 * temp                temperature in degree celsius   NULL on OpenBSD
 *                                                     NULL or thermal zone
 *                                                     on Linux
 *                                                     (hwmon0, hwmon1, etc.)
 *                                                     thermal zone on FreeBSD
 *                                                     (tz0, tz1, etc.)
 * uid                 UID of current user             NULL
 * uptime              system uptime                   NULL
 * username            username of current user        NULL
 * vol_perc            OSS/ALSA volume in percent      mixer file (/dev/mixer)
 * wifi_perc           WiFi signal in percent          interface name (wlan0)
 * wifi_essid          WiFi ESSID                      interface name (wlan0)
 *
 * EXTRA INFO:
 *
 * - every arg must ends with `END`
 *
 * - if you want to run function once (for example `hostname`),
 *   then set interval to `ONCE`
 *
 * EXTRA CONFIGS IN:
 *   - components_config.h
 */


/* for usleep */
#define SEC * 1000
#define MIN * 60 SEC

#define ONCE ((unsigned int) -1)  /* to run */

/* text to show if no value can be retrieved */
static const char unknown_str[] = "n/a";

/* maximum output string length */
#define MAXLEN 256
/*
 * if you want to change buffer size for each segment,
 * then change `BUFF_SZ` in util.h
 */

static const char IFC[] = "wlan0";  /* wifi interface */


static struct arg_t args[] = {

/* function		format		argument	interval (in ms) */

#if USE_X
/* { bspwm_ws,		" [ %s ]%%{r}",	NULL,		0,	END }, */
#endif
{ disk_perc,		" [# %s%%",	"/",		25 SEC,	END },
{ disk_free,		" %s]",		"/",		25 SEC,	END },
{ cpu_perc,		"-[ %3s%%]",	NULL,		 1 SEC,	END },
{ load_avg,		"-[%s]",	NULL,		 3 SEC,	END },
{ ram_used,		"-[﬙ %s]", 	NULL,		 3 SEC,	END },
{ vol_perc,		"-[%s]",	NULL,		 0,	END },
{ wifi_essid,		"-[直 \"%s\"",	IFC,		 2 SEC,	END },
{ wifi_perc,		" %3s%%]",	IFC,		 2 SEC,	END },
{ battery_state,	"-[%s",		"BAT0",		 2 SEC,	END },
{ battery_perc,		" %3s%%]",	"BAT0",		 1 MIN,	END },
{ datetime,		"-[ %s]",	"%H:%M:%S",	 1 SEC,	END },
#if USE_X && USE_XKB
{ keymap,		"-[ %s] ",	NULL,		 0,	END },
#endif

};
