#ifndef _COMPONENTS_VOLUME_H
#define _COMPONENTS_VOLUME_H

#if defined(USE_ALSA) || defined(USE_PULSE)
static const char SYM[]	    = "";      /* you can specify volume sym there */
static const char PERCENT[] = " %";    /* percent sign */
static const char MUTED[]   = "muted"; /* string to be displayed if muted */
#endif

#if defined(USE_ALSA)
#	include <alsa/asoundlib.h>
#	include <alsa/control.h>
struct volume_static_data {
	snd_mixer_selem_id_t *sid;
	struct {
		long int min, max;
	} volume;
	snd_ctl_event_t *e;
	snd_ctl_t *	 ctl;
};
#	define VOLUME_STATIC_SIZE sizeof(struct volume_static_data)

#elif defined(USE_PULSE)
#	include <stdint.h>
#	include <stdbool.h>
#	include <pthread.h>
#	include <pulse/pulseaudio.h>

struct volume_static_data {
	char*	  out;
	pthread_t volume_thread;
	uint8_t	  pulse_thread_started;
};
#	define VOLUME_STATIC_SIZE sizeof(struct volume_static_data)

#else
#	define VOLUME_STATIC_SIZE 0
#endif

#endif /* _COMPONENTS_VOLUME_H */
