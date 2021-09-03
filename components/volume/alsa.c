#include <err.h>
#include <math.h>
#include <inttypes.h>

#include <alsa/asoundlib.h>
#include <alsa/control.h>
/* header file inclusion order is important */

#ifndef USE_ALSA
#define USE_ALSA
#endif
#include "volume.h"
#include "../../util.h"
#include "../../components_config.h"

#ifndef VOLUME_SYM
#	define VOLUME_SYM ""
#endif

#ifndef VOLUME_PERCENT
#	define VOLUME_PERCENT " %"
#endif

#ifndef VOLUME_MUTED
#	define VOLUME_MUTED "muted"
#endif

#ifndef VOLUME_ALSA_CARD
#	define VOLUME_ALSA_CARD "default"
#endif

#ifndef VOLUME_ALSA_MIXER_NAME
#	define VOLUME_ALSA_MIXER_NAME "Master"
#endif

static const size_t CTL_NAME_MAX = 3 + 10 + 1;
/*
	3  - "hw:"
	10 - len(str(UINT_MAX))
	1  - zero byte
*/

typedef struct volume_static_data static_data;

static inline snd_mixer_t *
get_mixer_elem(snd_mixer_elem_t **ret, snd_mixer_selem_id_t **sid)
/*
	after using `mixer_elem`
	to free memory returned `mixer` must be closed with:
	`snd_mixer_close`

	(see `is_muted` function)
*/
{
	int	     err;
	snd_mixer_t *handle;

	if (!*sid) {
		if ((err = snd_mixer_selem_id_malloc(sid)) < 0) {
			warnx("failed to allocate memory for: %s",
			      snd_strerror(err));
			return NULL;
		}
		snd_mixer_selem_id_set_name(*sid, VOLUME_ALSA_MIXER_NAME);
	}

	if ((err = snd_mixer_open(&handle, 0)) < 0) {
		warnx("cannot open mixer: %s", snd_strerror(err));
		return NULL;
	}
	if ((err = snd_mixer_attach(handle, VOLUME_ALSA_CARD)) < 0) {
		warnx("cannot attach mixer: %s", snd_strerror(err));
		snd_mixer_close(handle);
		return NULL;
	}
	if ((err = snd_mixer_selem_register(handle, NULL, NULL)) < 0) {
		warnx("cannot register mixer: %s", snd_strerror(err));
		snd_mixer_close(handle);
		return NULL;
	}
	if ((err = snd_mixer_load(handle)) < 0) {
		warnx("failed to load mixer: %s", snd_strerror(err));
		snd_mixer_close(handle);
		return NULL;
	}

	*ret = snd_mixer_find_selem(handle, *sid);

	return handle;
}

static inline uint8_t
is_muted(snd_mixer_selem_id_t **sid)
{
	int		  psw;
	snd_mixer_t *	  handle;
	snd_mixer_elem_t *elem;

	if (!(handle = get_mixer_elem(&elem, sid))) return 0;

	snd_mixer_selem_get_playback_switch(elem, SND_MIXER_SCHN_MONO, &psw);
	snd_mixer_close(handle);

	return !psw;
}

static inline unsigned short int
get_percentage(__typeof__(((static_data *)0)->volume) *v,
	       snd_mixer_selem_id_t **		       sid)
{
	int		  err;
	long int	  vol;
	snd_mixer_t *	  handle;
	snd_mixer_elem_t *elem;

	if (!(handle = get_mixer_elem(&elem, sid))) return 0;

	if (!v->max)
		snd_mixer_selem_get_playback_volume_range(elem,
							  &v->min,
							  &v->max);

	err = snd_mixer_selem_get_playback_volume(elem, 0, &vol);
	snd_mixer_close(handle);
	if (err < 0) {
		warnx("cannot get playback volume: %s", snd_strerror(err));
		return 0;
	}

	return (unsigned short int)((vol - v->min) * 100 / (v->max - v->min));
}

static inline char *
get_ctl_name(snd_mixer_selem_id_t **sid)
/* after using return must be freed */
{
	char *		  ctl_name;
	unsigned int	  index;
	snd_mixer_t *	  handle;
	snd_mixer_elem_t *elem;

	if (!(handle = get_mixer_elem(&elem, sid))) {
		index = 0;
	} else {
		index = snd_mixer_selem_get_index(elem);
		snd_mixer_close(handle);
	}
	if (!(ctl_name = calloc(CTL_NAME_MAX, 1))) {
		warnx("failed to allocate memory for ctl_name");
		return NULL;
	}
	snprintf(ctl_name, CTL_NAME_MAX, "hw:%u", index);
	return ctl_name;
}

void
vol_perc(char *	    volume,
	 const char __unused * _a,
	 unsigned int __unused _i,
	 void *		       static_ptr)
{
	int	     err;
	char *	     ctl_name;
	static_data *data = static_ptr;

	if (!data->ctl) {
		if (!(ctl_name = get_ctl_name(&data->sid))) ERRRET(volume);

		snd_ctl_open(&data->ctl, ctl_name, SND_CTL_READONLY);
		free(ctl_name);

		err = snd_ctl_subscribe_events(data->ctl, 1);
		if (err < 0) {
			snd_ctl_close(data->ctl);
			data->ctl = NULL;
			warnx("cannot subscribe to alsa events: %s",
			      snd_strerror(err));
			ERRRET(volume);
		}
		snd_ctl_event_malloc(&data->e);
	} else {
		snd_ctl_read(data->ctl, data->e);
	}

	if (is_muted(&data->sid))
		bprintf(volume, "%s", VOLUME_MUTED);
	else
		bprintf(volume,
			"%s%3hu%s",
			VOLUME_SYM,
			get_percentage(&data->volume, &data->sid),
			VOLUME_PERCENT);
}
