/*
 * for components info see: wiki/COMPONENTS.md
 *
 * extra configs in can be found in `components_config.h`
 */

/* for usleep */
#define _SEC *1000
#define _MIN *(60 _SEC)
#define _HR  *(60 _MIN)

/* interval to run only once */
#define ONCE ((unsigned int)-1)

/* text to show if no value can be retrieved */
static const char unknown_str[] = "n/a";

/* maximum output string length */
#define MAXLEN 256
/*
 * if you want to change buffer size for each segment,
 * then change `BUFF_SZ` in lib/util.h
 */

#define IFC "wlan0" /* wifi interface */

/* clang-format off */
static struct arg_t args[] = {

/* function		format		argument	interval (in ms) */

#if USE_X
/* { bspwm_ws,		" [ %s ]%%{r}",	NULL,		0,	END }, */
#endif
{ disk_perc,		" [# %s%%",	"/",		25 _SEC,	END },
{ disk_free,		" %s]",		"/",		25 _SEC,	END },
{ cpu_perc,		"-[ %3s%%]",	NULL,		 1 _SEC,	END },
{ load_avg,		"-[%s]",	NULL,		 3 _SEC,	END },
{ ram_used,		"-[﬙ %s]", 	NULL,		 3 _SEC,	END },
{ vol_perc,		"-[%s]",	NULL,		 0,		END },
{ wifi_essid,		"-[直 \"%s\"",	IFC,		 2 _SEC,	END },
{ wifi_perc,		" %3s%%]",	IFC,		 2 _SEC,	END },
{ battery_state,	"-[%s",		"BAT0",		 2 _SEC,	END },
{ battery_perc,		" %3s%%]",	"BAT0",		 1 _MIN,	END },
{ datetime,		"-[ %s]",	"%H:%M:%S",	 1 _SEC,	END },
#if USE_X && USE_XKB
{ keymap,		"-[ %s] ",	NULL,		 0,	END },
#endif

};
/* clang-format on */
