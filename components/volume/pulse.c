#include "../../thread_helper.h"

#include <err.h>
#include <time.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>

#include <pulse/pulseaudio.h>

#ifndef USE_PULSE
#	define USE_PULSE
#endif
#include "../volume.h"
#include "../../lib/util.h"
#include "../../aslstatus.h"
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

#ifndef VOLUME_PULSE_PROC_NAME
#	define VOLUME_PULSE_PROC_NAME "volume:pulse"
#endif

#ifndef VOLUME_PULSE_RECONECT_TIMEOUT
#	define VOLUME_PULSE_RECONECT_TIMEOUT 5000
#endif

static void *mainloop_thread(void *);
static void  pulse_cleanup(void *ptr);

void
vol_perc(char		      *volume,
	 const char __unused *_a,
	 uint32_t __unused    _i,
	 static_data_t       *static_data)
{
	struct volume_static_data	  *data = static_data->data;
	static const struct timespec ts	  = {
		  .tv_sec  = MS2S(VOLUME_PULSE_RECONECT_TIMEOUT),
		  .tv_nsec = MS2NS(VOLUME_PULSE_RECONECT_TIMEOUT),
	};

	if (!static_data->cleanup) static_data->cleanup = pulse_cleanup;

	if (!data->pulse_thread_started) {
		data->pulse_thread_started = !0;

		data->out		   = volume;
		data->volume_thread	   = pthread_self();

		pthread_create(&data->pulse_thread,
			       NULL,
			       mainloop_thread,
			       data);
		pthread_setname(data->pulse_thread, VOLUME_PULSE_PROC_NAME);
	} else {
		for (;;) {
			if (!!data->pulse_thread
			    && !pthread_tryjoin_np(data->pulse_thread, NULL)) {
				data->pulse_thread_started = 0;
				ERRRET(volume);
			}

			if (!!nanosleep(&ts, NULL))
				/* if interrupted by signal */
				break;
		}
	}
}

static void
quit(int ret, pa_mainloop_api *mainloop)
{
	assert(mainloop);
	mainloop->quit(mainloop, ret);
}

static void
sink_info_callback(pa_context __unused *_c,
		   const pa_sink_info  *i,
		   int __unused		_is_last,
		   void		*userdata)
{
	struct volume_static_data *data = userdata;

	if (!i) return;

	if (i->mute)
		bprintf(data->out, "%s", VOLUME_MUTED);
	else
		bprintf(data->out,
			"%s%" PRIperc "%s",
			VOLUME_SYM,
			(percent_t)((pa_cvolume_avg(&(i->volume)) * 100
				     + (pa_volume_t)PA_VOLUME_NORM / 2)
				    / (pa_volume_t)PA_VOLUME_NORM),
			VOLUME_PERCENT);

	pthread_kill(data->volume_thread, SIGUSR1);
}

static void
subscribe_callback(pa_context		      *c,
		   pa_subscription_event_type_t type,
		   uint32_t			idx,
		   void			*userdata)
{
	switch (type & PA_SUBSCRIPTION_EVENT_FACILITY_MASK) {
	case PA_SUBSCRIPTION_EVENT_SINK:
		pa_operation_unref(
		    pa_context_get_sink_info_by_index(c,
						      idx,
						      sink_info_callback,
						      userdata));
	}
}

static void
server_info_callback(pa_context *c, const pa_server_info *i, void *userdata)
{
	pa_context_get_sink_info_by_name(c,
					 i->default_sink_name,
					 sink_info_callback,
					 userdata);
}

static void
context_state_callback(pa_context *c, void *userdata)
{
	struct volume_static_data *data = userdata;

	switch (pa_context_get_state(c)) {
	case PA_CONTEXT_CONNECTING:
	case PA_CONTEXT_AUTHORIZING:
	case PA_CONTEXT_SETTING_NAME:
		break;

	case PA_CONTEXT_READY:
		pa_context_get_server_info(c, server_info_callback, data);
		/*
		 * Subscribe to sink events from the server. This is how we get
		 * volume change notifications from the server.
		 */
		pa_context_set_subscribe_callback(c, subscribe_callback, data);
		pa_context_subscribe(c, PA_SUBSCRIPTION_MASK_SINK, NULL, NULL);
		break;

	case PA_CONTEXT_TERMINATED:
		warnx("PulseAudio connection terminated.\n");
		quit(!0, data->mainloop);
		break;

	case PA_CONTEXT_FAILED:
	default:
		warnx("Connection failure: %s",
		      pa_strerror(pa_context_errno(c)));
		quit(!0, data->mainloop);
	}
}

void *
mainloop_thread(void *userdata)
{
	pa_mainloop		    *m;
	int			   ret	   = 1;
	pa_context		   *context = NULL;
	struct volume_static_data *data	   = userdata;

	if (!(m = pa_mainloop_new())) {
		warnx("pa_mainloop_new() failed");
		goto quit;
	}

	data->mainloop = pa_mainloop_get_api(m);

	assert(pa_signal_init(data->mainloop) == 0);

	if (!(context = pa_context_new(data->mainloop,
				       "Aslstatus pulseaudio widget"))) {
		warnx("pa_context_new() failed");
		goto quit;
	}

	if (pa_context_connect(context, NULL, PA_CONTEXT_NOAUTOSPAWN, NULL)
	    < 0) {
		warnx("pa_context_connect() failed: %s",
		      pa_strerror(pa_context_errno(context)));
		goto quit;
	}

	pa_context_set_state_callback(context,
				      context_state_callback,
				      userdata);

	if (pa_mainloop_run(m, &ret) < 0) warnx("pa_mainloop_run() failed");

quit:
	if (!!context) pa_context_unref(context);

	if (!!m) {
		pa_signal_done();
		pa_mainloop_free(m);
	}

	return (data->mainloop = NULL);
}

static inline void
pulse_cleanup(void *ptr)
{
	struct volume_static_data *data = ptr;

	pthread_cancel(data->volume_thread);
	if (!!data->mainloop) quit(0, data->mainloop);
	pthread_join(data->volume_thread, NULL);
}
