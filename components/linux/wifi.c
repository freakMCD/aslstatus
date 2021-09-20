#include <err.h>
#include <string.h>

#include <sys/ioctl.h>
#include <sys/socket.h>

#include <linux/wireguard.h>

#include "../wifi.h"
#include "../../lib/util.h"
#include "../../aslstatus.h"

#define CLEANUP(X)                                                            \
	static inline void wifi_##X##_cleanup(void *ptr)                      \
	{                                                                     \
		eclose(((struct wifi_##X##_data *)ptr)->common.sock);         \
	}                                                                     \
	struct trailing_semicolon

static void wifi_perc_cleanup(void *ptr);
static void wifi_essid_cleanup(void *ptr);

void
wifi_perc(char *		out,
	  const char *		interface,
	  unsigned int __unused _i,
	  static_data_t *	static_data)
{
	struct wifi_perc_data *data = static_data->data;

	if (!static_data->cleanup) static_data->cleanup = wifi_perc_cleanup;

	if (data->common.sock <= 0) {
		EFUNC(
		    0 >,
		    { ERRRET(out); },
		    data->common.sock,
		    socket,
		    AF_INET,
		    SOCK_DGRAM,
		    0);

		strncpy(data->common.req.ifr_name,
			interface,
			sizeof(data->common.req.ifr_name));
		data->common.req.u.data.pointer = &data->stat;
		data->common.req.u.data.length	= sizeof(data->stat);
	}

	EFUNC(
	    -1 == (int),
	    {
		    close(data->common.sock);
		    data->common.sock = -1;
		    ERRRET(out);
	    },
	    _unused,
	    ioctl,
	    data->common.sock,
	    SIOCGIWSTATS,
	    &data->common.req);

	bprintf(out,
		"%hhu",
		(uint8_t)((float)data->stat.qual.qual / (70 /* max RSSI */)
			  * 100));
}

void
wifi_essid(char *		 out,
	   const char *		 interface,
	   unsigned int __unused _i,
	   static_data_t *	 static_data)
{
	struct wifi_essid_data *data = static_data->data;

	if (!static_data->cleanup) static_data->cleanup = wifi_essid_cleanup;

	if (data->common.sock <= 0) {
		EFUNC(
		    0 >,
		    { ERRRET(out); },
		    data->common.sock,
		    socket,
		    AF_INET,
		    SOCK_DGRAM,
		    0);

		strncpy(data->common.req.ifr_name,
			interface,
			sizeof(data->common.req.ifr_name));
		data->common.req.u.essid.pointer = data->id;
		data->common.req.u.essid.length	 = sizeof(data->id);
	}

	EFUNC(
	    -1 == (int),
	    {
		    close(data->common.sock);
		    data->common.sock = -1;
		    ERRRET(out);
	    },
	    _unused,
	    ioctl,
	    data->common.sock,
	    SIOCGIWESSID,
	    &data->common.req);

	if (!*data->id) ERRRET(out);

	bprintf(out, "%s", data->id);
}

CLEANUP(perc);
CLEANUP(essid);
