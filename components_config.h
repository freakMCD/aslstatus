#ifndef _COMPONENTS_CONFIG_H
#define _COMPONENTS_CONFIG_H

/* some settings for components */

/*
 * KEYMAP
 *
 * default: ""
 */

/* #define KEYMAP_NUMLOCK " NL" */

/* BATTERY */
#define BATTERY_FULL	    ""	 /* "f" */
#define BATTERY_UNKNOWN	    ""	 /* "?" */
#define BATTERY_CHARGING    "+" /* "+" */
#define BATTERY_DISCHARGING "-" /* "-" */

/* BSPWM */
#define BSPWM_FOCUSED_RESET "%{F-}%{B-}"
#define BSPWM_FOCUSED_FG    "#0ff"
#define BSPWM_FOCUSED_BG    "-"
#define BSPWM_DELIM	    ' ' /* must be CHAR */
/* define to show vacant tags */
/* #define BSPWM_SHOW_VACANT_TAGS */

/* VOLUME */
#define VOLUME_SYM	       "墳 "   /* you can specify volume sym there */
#define VOLUME_PERCENT	       " %"    /* percent sign */
#define VOLUME_MUTED	       "muted" /* string to be displayed if muted */
#define VOLUME_ALSA_CARD       "default"
#define VOLUME_ALSA_MIXER_NAME "Master"
#define VOLUME_PULSE_PROC_NAME "volume:pulse" /* PulseAudio thread name */

#endif /* _COMPONENTS_CONFIG_H */
