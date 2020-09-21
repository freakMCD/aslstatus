/* See LICENSE file for copyright and license details. */
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include "../util.h"

#if defined(USE_ALSA) || defined(USE_PULSE)
static const char SYM[] = "";         /* you can specify volume sym there */
static const char PERCENT[] = " %";   /* percent sign */
static const char MUTED[] = "muted";  /* string to be displayed if muted */
#endif

#if defined(USE_ALSA)
	#include <math.h>
	#include <stdbool.h>

	#include <alsa/asoundlib.h>
	#include <alsa/control.h>
	/* header file inclusion order is important */

	static inline void
	get_volume_range(long *min, long *max)
	{
		snd_mixer_t *handle;
		snd_mixer_elem_t *elem;
		snd_mixer_selem_id_t *sid;

		snd_mixer_selem_id_alloca(&sid);

		snd_mixer_open(&handle, 0);
		snd_mixer_attach(handle, "default");
		snd_mixer_selem_register(handle, NULL, NULL);
		snd_mixer_load(handle);

		snd_mixer_selem_id_set_index(sid, 0);
		snd_mixer_selem_id_set_name(sid, "Master");
		elem = snd_mixer_find_selem(handle, sid);

		snd_mixer_selem_get_playback_volume_range(elem, min, max);
		snd_mixer_close(handle);
	}


	static inline bool
	is_muted(void)
	{
		int psw;
		snd_mixer_t *handle;
		snd_mixer_selem_id_t *sid;

		snd_mixer_selem_id_alloca(&sid);

		snd_mixer_open(&handle, 0);
		snd_mixer_attach(handle, "default");
		snd_mixer_selem_register(handle, NULL, NULL);
		snd_mixer_load(handle);

		snd_mixer_selem_id_set_index(sid, 0);
		snd_mixer_selem_id_set_name(sid, "Master");
		snd_mixer_elem_t *elem = snd_mixer_find_selem(handle, sid);

		snd_mixer_selem_get_playback_switch(elem,
				SND_MIXER_SCHN_MONO, &psw);

		snd_mixer_close(handle);

		return !psw;
	}


	void
	vol_perc(char *volume, const char *unused)
	{
		long int
			min = 0,
			max = 0;
		unsigned char vol;

		static long int range = 0;
		static snd_ctl_event_t *e;
		static snd_ctl_t *ctl = NULL;
		static snd_ctl_elem_id_t *id;
		static snd_ctl_elem_value_t *control;

		if (!ctl) {
			snd_ctl_open(&ctl, "hw:0", SND_CTL_READONLY);
			if (snd_ctl_subscribe_events(ctl, 1) < 0) {
				snd_ctl_close(ctl);
				ERRRET(volume);
			}

			snd_ctl_event_malloc(&e);
			snd_ctl_elem_id_alloca(&id);
			snd_ctl_elem_value_malloc(&control);

			snd_ctl_elem_id_set_interface(id,
					SND_CTL_ELEM_IFACE_MIXER);
			snd_ctl_elem_id_set_name(id, "Master Playback Volume");
			snd_ctl_elem_value_set_id(control, id);

			get_volume_range(&min, &max);
			range = max - min;
		} else {
			snd_ctl_read(ctl, e);
		}

		if (is_muted()) {
			bprintf(volume, "%s", MUTED);
		} else {
			snd_ctl_elem_read(ctl, control);
			vol = snd_ctl_elem_value_get_integer(control, 0);
			vol = rint((double)vol / (double)range * 100);
			bprintf(volume, "%s%3hhu%s",
					SYM,
					vol * (range > 0),
					PERCENT);
		}
	}

#elif defined(USE_PULSE)
/* !!! in progress !!! */
/*
	#include <signal.h>
	#include <assert.h>
	#include <stdlib.h>
	#include <stdbool.h>

	#include <pthread.h>
	#include <pulse/pulseaudio.h>


	static char *server = NULL;
	static pa_context *context = NULL;
	static pa_proplist *proplist = NULL;
	static pa_mainloop_api *mainloop_api = NULL;

	static pthread_t updater = NULL;
	static volatile bool done = false;
	static char volume[LEN(MUTED) > 3 ? LEN(MUTED) :3] = {0};

	static inline void
	update_volume(int s)
	{ (void) s; }

	static inline void
	quit(int ret)
	{
		assert(mainloop_api);
		mainloop_api->quit(mainloop_api, ret);
	}


	static inline void
	exit_signal_callback(pa_mainloop_api *m, pa_signal_event *e, int sig,
			void *userdata)
	{
		fprintf(stderr, "Got signal: %d, exiting.\n", sig);
		quit(0);
	}

*/

#elif defined(__OpenBSD__)
	#include <sys/audioio.h>

	void
	vol_perc(char *out, const char *card)
	{
		static int cls = -1;
		mixer_devinfo_t mdi;
		mixer_ctrl_t mc;
		int afd = -1, m = -1, v = -1;

		if ((afd = open(card, O_RDONLY)) < 0) {
			warn("open '%s':", card);
			ERRRET(out);
		}

		for (mdi.index = 0; cls == -1; mdi.index++) {
			if (ioctl(afd, AUDIO_MIXER_DEVINFO, &mdi) < 0) {
				warn("ioctl 'AUDIO_MIXER_DEVINFO':");
				close(afd);
				ERRRET(out);
			}
			if (mdi.type == AUDIO_MIXER_CLASS &&
			    !strncmp(mdi.label.name,
				     AudioCoutputs,
				     MAX_AUDIO_DEV_LEN))
				cls = mdi.index;
			}
		for (mdi.index = 0; v == -1 || m == -1; mdi.index++) {
			if (ioctl(afd, AUDIO_MIXER_DEVINFO, &mdi) < 0) {
				warn("ioctl 'AUDIO_MIXER_DEVINFO':");
				close(afd);
				ERRRET(out);
			}
			if (mdi.mixer_class == cls &&
			    ((mdi.type == AUDIO_MIXER_VALUE &&
			      !strncmp(mdi.label.name,
				       AudioNmaster,
				       MAX_AUDIO_DEV_LEN)) ||
			     (mdi.type == AUDIO_MIXER_ENUM &&
			      !strncmp(mdi.label.name,
				      AudioNmute,
				      MAX_AUDIO_DEV_LEN)))) {
				mc.dev = mdi.index, mc.type = mdi.type;
				if (ioctl(afd, AUDIO_MIXER_READ, &mc) < 0) {
					warn("ioctl 'AUDIO_MIXER_READ':");
					close(afd);
					ERRRET(out);
				}
				if (mc.type == AUDIO_MIXER_VALUE)
					v = mc.un.value.num_channels == 1 ?
					    mc.un.value.level[AUDIO_MIXER_LEVEL_MONO] :
					    (mc.un.value.level[AUDIO_MIXER_LEVEL_LEFT] >
					     mc.un.value.level[AUDIO_MIXER_LEVEL_RIGHT] ?
					     mc.un.value.level[AUDIO_MIXER_LEVEL_LEFT] :
					     mc.un.value.level[AUDIO_MIXER_LEVEL_RIGHT]);
				else if (mc.type == AUDIO_MIXER_ENUM)
					m = mc.un.ord;
			}
		}

		close(afd);

		bprintf(out, "%3d", m ? 0 : v * 100 / 255);
	}
#else
	#include <sys/soundcard.h>

	void
	vol_perc(char * out, const char *card)
	{
		size_t i;
		int v, afd, devmask;
		char *vnames[] = SOUND_DEVICE_NAMES;

		if ((afd = open(card, O_RDONLY | O_NONBLOCK)) < 0) {
			warn("open '%s':", card);
			ERRRET(out);
		}

		if (ioctl(afd, (int)SOUND_MIXER_READ_DEVMASK, &devmask) < 0) {
			warn("ioctl 'SOUND_MIXER_READ_DEVMASK':");
			close(afd);
			ERRRET(out);
		}
		for (i = 0; i < LEN(vnames); i++) {
			if (devmask & (1 << i) && !strcmp("vol", vnames[i])) {
				if (ioctl(afd, MIXER_READ(i), &v) < 0) {
					warn("ioctl 'MIXER_READ(%ld)':", i);
					close(afd);
					ERRRET(out);
				}
			}
		}

		close(afd);

		bprintf(out, "%3d", v & 0xff);
	}
#endif
