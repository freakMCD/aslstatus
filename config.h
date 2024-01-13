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
#define MAXLEN 512
/*
 * if you want to change buffer size for each segment,
 * then change `BUFF_SZ` in lib/util.h
 */

/* clang-format off */
static struct arg_t args[] = {

#define RED "^c#ea6962^"
#define ORANGE "^c#e78a4e^"
#define YELLOW "^c#d8a657^"
#define GREEN "^c#a9b665^"
#define AQUA "^c#89b482^"
#define BLUE "^c#7daea3^"
#define PURPLE "^c#d3869b^"
#define GREY "^c#7c6f64^"

/* function		format		argument	interval (in ms) */
#if USE_X
/* { bspwm_ws,		" [ %s ]%%{r}",	NULL,		0,	END }, */
#endif

{ run_command,  " %s  ", "sh -c ~/.config/aslstatus/scripts/song.sh", ONCE, END },
{ run_command,  AQUA"║ %s ", "curl -s wttr.in?format='%x+%t'", 1 _HR, END },
{ ipv4,         GREEN"║ ^d^%s ", "enp34s0",      3 _SEC, END },
{ cpu_perc,		YELLOW"║ CPU ^d^%2s%% ",	NULL,	1 _SEC,	END },
{ temp,		    "%sºC ",	NULL,		    1 _SEC,	END },
{ ram_used,		YELLOW"║ RAM ^d^%s ", 	NULL,		3 _SEC,	END },
{ disk_free,	YELLOW"║ SSD ^d^%s ",		"/",   25 _SEC,	END },
{ datetime,		YELLOW"║ %s ", "%b %d ^d^%a %T",	    1 _SEC,	END },

};
/* clang-format on */
