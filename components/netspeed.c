#include <err.h>
#include <stdio.h>
#include <limits.h>
#include <stdint.h>

#include "netspeed.h"
#include "../lib/util.h"
#include "../aslstatus.h"

#define SYS_CLASS "/sys/class/net"

#define STATISTICS(S) "statistics/" #S "_bytes"

static void netspeed(char *	    out,
		     const char *   interface,
		     uint32_t	    interval,
		     static_data_t *static_data,
		     const char *   property);

void
netspeed_rx(char *out, const char *interface, uint32_t interval, void *ptr)
{
	netspeed(out, interface, interval, ptr, STATISTICS(rx));
}

void
netspeed_tx(char *out, const char *interface, uint32_t interval, void *ptr)
{
	netspeed(out, interface, interval, ptr, STATISTICS(tx));
}

static inline void
netspeed_cleanup(void *ptr)
{
	eclose(((struct netspeed_data *)ptr)->fd);
}

static inline void
netspeed(char *		out,
	 const char *	interface,
	 uint32_t	interval,
	 static_data_t *static_data,
	 const char *	property)
{
	struct netspeed_data *data     = static_data->data;
	uintmax_t	      oldbytes = data->bytes;
	char		      buf[JU_STR_SIZE];

	if (!static_data->cleanup) static_data->cleanup = netspeed_cleanup;

	if (!sysfs_fd_or_rewind(&data->fd, SYS_CLASS, interface, property))
		ERRRET(out);

	if (!eread(data->fd, WITH_LEN(buf))) ERRRET(out);

	if (!esscanf(1, buf, "%ju", &data->bytes)) ERRRET(out);

	if (!oldbytes) ERRRET(out);

	fmt_human(out, (data->bytes - oldbytes) * 1000 / interval);
}
