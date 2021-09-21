#include <err.h>
#include <string.h>

#include <sys/ioctl.h>
#include <sys/socket.h>

#include <linux/wireguard.h>

#include "wifi.h"
#include "../lib/util.h"
#include "../aslstatus.h"

#define CLEANUP(X)                                                            \
	static inline void wifi_##X##_cleanup(void *ptr)                      \
	{                                                                     \
		eclose(((struct wifi_##X##_data *)ptr)->common.sock);         \
	}                                                                     \
	struct trailing_semicolon

#define eioctl(fd, req, arg) _eioctl(__func__, (fd), (req), #req, arg)
static uint8_t
_eioctl(const char *func, int fd, int req, const char *str_req, void *arg);

#define esocket(socket, ...)                                                  \
	((socket = _esocket(__func__, #__VA_ARGS__, __VA_ARGS__)) != -1)
static int
_esocket(const char *func, const char *args, int domain, int type, int proto);

#define SOCKET_INIT(DATA, OUT)                                                \
	do {                                                                  \
		if (!esocket((DATA)->common.sock, AF_INET, SOCK_DGRAM, 0))    \
			ERRRET(OUT);                                          \
	} while (0)

static void wifi_perc_cleanup(void *ptr);
static void wifi_essid_cleanup(void *ptr);

void
wifi_perc(char *	    out,
	  const char *	    interface,
	  uint32_t __unused _i,
	  static_data_t *   static_data)
{
	struct wifi_perc_data *data = static_data->data;

	if (!static_data->cleanup) static_data->cleanup = wifi_perc_cleanup;

	if (data->common.sock <= 0) {
		SOCKET_INIT(data, out);

		strncpy(data->common.req.ifr_name,
			interface,
			sizeof(data->common.req.ifr_name));
		data->common.req.u.data.pointer = &data->stat;
		data->common.req.u.data.length	= sizeof(data->stat);
	}

	if (!eioctl(data->common.sock, SIOCGIWSTATS, &data->common.req)) {
		close(data->common.sock);
		data->common.sock = -1;
		ERRRET(out);
	}

	bprintf(out,
		"%" PRIperc,
		(percent_t)((float)data->stat.qual.qual / (70 /* max RSSI */)
			    * 100));
}

void
wifi_essid(char *	     out,
	   const char *	     interface,
	   uint32_t __unused _i,
	   static_data_t *   static_data)
{
	struct wifi_essid_data *data = static_data->data;

	if (!static_data->cleanup) static_data->cleanup = wifi_essid_cleanup;

	if (data->common.sock <= 0) {
		SOCKET_INIT(data, out);

		strncpy(data->common.req.ifr_name,
			interface,
			sizeof(data->common.req.ifr_name));
		data->common.req.u.essid.pointer = data->id;
		data->common.req.u.essid.length	 = sizeof(data->id);
	}

	if (!eioctl(data->common.sock, SIOCGIWESSID, &data->common.req)) {
		close(data->common.sock);
		data->common.sock = -1;
		ERRRET(out);
	}

	if (!*data->id) ERRRET(out);

	bprintf(out, "%s", data->id);
}

static inline uint8_t
_eioctl(const char *func, int fd, int req, const char *str_req, void *arg)
{
	uint8_t ret;

	if (!(ret = (ioctl(fd, req, arg) != -1)))
		warn("%s: ioctl(%d, %s, %p)", func, fd, str_req, arg);

	return ret;
}

static inline int
_esocket(const char *func, const char *args, int domain, int type, int proto)
{
	int ret;

	if ((ret = socket(domain, type, proto)) == -1)
		warn("%s: socket(%s)", func, args);

	return ret;
}

CLEANUP(perc);
CLEANUP(essid);

