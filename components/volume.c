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
	static const size_t CTL_NAME_MAX = 3 + 10 + 1;
	/*
		3  - "hw:"
		10 - len(str(UINT_MAX))
		1  - zero byte
	*/
	static const char CARD[] = "default";
	static const char MIXER_NAME[] = "Master";

	#include <math.h>
	#include <stdbool.h>

	#include <alsa/asoundlib.h>
	#include <alsa/control.h>
	/* header file inclusion order is important */


	static inline snd_mixer_t *
	get_mixer_elem(snd_mixer_elem_t **ret)
	/*
		after using `mixer_elem`
		to free memory returned `mixer` must be closed with:
		`snd_mixer_close`

		(see `is_muted` function)
	*/
	{
		int err;
		snd_mixer_t *handle;
		static snd_mixer_selem_id_t *sid;

		if (!sid) {
			if ((err = snd_mixer_selem_id_malloc(&sid)) < 0) {
				warn("failed to allocate memory for: %s",
						snd_strerror(err));
				return NULL;
			}
			snd_mixer_selem_id_set_name(sid, MIXER_NAME);
		}

		if ((err = snd_mixer_open(&handle, 0)) < 0) {
			warn("cannot open mixer: %s", snd_strerror(err));
			return NULL;
		}
		if ((err = snd_mixer_attach(handle, CARD)) < 0) {
			warn("cannot attach mixer: %s", snd_strerror(err));
			snd_mixer_close(handle);
			return NULL;
		}
		if ((err = snd_mixer_selem_register(handle, NULL, NULL)) < 0) {
			warn("cannot register mixer: %s", snd_strerror(err));
			snd_mixer_close(handle);
			return NULL;
		}
		if ((err = snd_mixer_load(handle)) < 0) {
			warn("failed to load mixer: %s", snd_strerror(err));
			snd_mixer_close(handle);
			return NULL;
		}

		*ret = snd_mixer_find_selem(handle, sid);

		return handle;
	}


	static inline bool
	is_muted(void)
	{
		int psw;
		snd_mixer_t *handle;
		snd_mixer_elem_t *elem;

		if (!(handle = get_mixer_elem(&elem)))
			return 0;

		snd_mixer_selem_get_playback_switch(elem,
				SND_MIXER_SCHN_MONO, &psw);
		snd_mixer_close(handle);

		return !psw;
	}


	static inline unsigned short int
	get_percentage(void)
	{
		int err;
		long int vol;
		static long int
			min = 0,
			max = 0;
		snd_mixer_t *handle;
		snd_mixer_elem_t *elem;

		if (!(handle = get_mixer_elem(&elem)))
			return 0;

		if (!max)
			snd_mixer_selem_get_playback_volume_range(
				elem,
				&min,
				&max
			);

		err = snd_mixer_selem_get_playback_volume(elem, 0, &vol);
		snd_mixer_close(handle);
		if (err < 0) {
			warn("cannot get playback volume: %s",
					snd_strerror(err));
			return 0;
		}
		return (unsigned short int)((vol - min) * 100 / (max - min));
	}


	static inline char *
	get_ctl_name(void)
	/* after using return must be freed */
	{
		char *ctl_name;
		unsigned int index;
		snd_mixer_t *handle;
		snd_mixer_elem_t *elem;

		if (!(handle = get_mixer_elem(&elem))) {
			index = 0;
		} else {
			index = snd_mixer_selem_get_index(elem);
			snd_mixer_close(handle);
		}
		if (!(ctl_name = calloc(CTL_NAME_MAX, 1))) {
			warn("failed to allocate memory for ctl_name");
			return NULL;
		}
		snprintf(ctl_name, CTL_NAME_MAX, "hw:%u", index);
		return ctl_name;
	}


	void
	vol_perc(char *volume)
	{
		int err;
		char *ctl_name;
		static snd_ctl_event_t *e;
		static snd_ctl_t *ctl = NULL;

		if (!ctl) {
			if (!(ctl_name = get_ctl_name()))
				ERRRET(volume);

			snd_ctl_open(&ctl, ctl_name, SND_CTL_READONLY);
			free(ctl_name);

			if ((err = snd_ctl_subscribe_events(ctl, 1)) < 0) {
				snd_ctl_close(ctl);
				ctl = NULL;
				warn("cannot subscribe to alsa events: %s",
						snd_strerror(err));
				ERRRET(volume);
			}
			snd_ctl_event_malloc(&e);
		} else {
			snd_ctl_read(ctl, e);
		}

		if (is_muted())
			bprintf(volume, "%s", MUTED);
		else
			bprintf(volume, "%s%3hu%s",
					SYM, get_percentage(), PERCENT);
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
