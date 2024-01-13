#ifndef _COMPONENTS_CONFIG_H
#define _COMPONENTS_CONFIG_H

/* some settings for components */

/*
 * for fmt_human from util.c
 *
 * NUMFMT_SI:  1000 = 1 K
 * NUMFMT_IEC: 1024 = 1 Ki
 */
#define FMT_HUMAN_NUMFMT NUMFMT_SI

/*
 * KEYMAP
 *
 * default: ""
 */

/* #define KEYMAP_NUMLOCK " NL" */

/* BATTERY */
#define BATTERY_FULL			  ""  /* "f" */
#define BATTERY_UNKNOWN			  ""  /* "?" */
#define BATTERY_CHARGING		  "+" /* "+" */
#define BATTERY_DISCHARGING		  "-" /* "-" */
#define BATTERY_REMAINING_NOT_DISCHARGING "?"

/* BSPWM */
#define BSPWM_FOCUSED_RESET "%{F-}%{B-}"
#define BSPWM_FOCUSED_FG    "#0ff"
#define BSPWM_FOCUSED_BG    "-"
#define BSPWM_DELIM	    ' ' /* must be CHAR */
/* define to show vacant tags */
/* #define BSPWM_SHOW_VACANT_TAGS */

/* ENTROPY */
/* Unicode Character 'INFINITY' (U+221E) */
#define ENTROPY_INFINITY "\xe2\x88\x9e" /* will be used only on BSD */

/* VOLUME */
#define VOLUME_SYM	       ""/* you can specify volume sym there */
#define VOLUME_PERCENT	       "%"    /* percent sign */
#define VOLUME_MUTED	       "muted"  /* string to be displayed if muted */
#define VOLUME_ALSA_CARD       "default"
#define VOLUME_ALSA_MIXER_NAME "Master"
#define VOLUME_PULSE_PROC_NAME "volume:pulse" /* PulseAudio thread name */

/* wait so many milliseconds before trying to reconnect to pulse server */
#define VOLUME_PULSE_RECONECT_TIMEOUT 5000

#endif /* _COMPONENTS_CONFIG_H */
