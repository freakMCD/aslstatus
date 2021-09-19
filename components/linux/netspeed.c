#include <err.h>
#include <stdio.h>
#include <limits.h>
#include <stdint.h>

#include "../netspeed.h"
#include "../../lib/util.h"
#include "../../aslstatus.h"

#define SYS_CLASS "/sys/class/net"

#define STATISTICS(S) "statistics/" #S "_bytes"

static void netspeed(char *	    out,
		     const char *   interface,
		     unsigned int   interval,
		     static_data_t *static_data,
		     const char *   property);

void
netspeed_rx(char *out, const char *interface, unsigned int interval, void *ptr)
{
	netspeed(out, interface, interval, ptr, STATISTICS(rx));
}

void
netspeed_tx(char *out, const char *interface, unsigned int interval, void *ptr)
{
	netspeed(out, interface, interval, ptr, STATISTICS(tx));
}

static inline void
netspeed_cleanup(void *ptr)
{
	CCLOSE(((struct netspeed_data_t *)ptr)->fd);
}

static inline void
netspeed(char *		out,
	 const char *	interface,
	 unsigned int	interval,
	 static_data_t *static_data,
	 const char *	property)
{
	struct netspeed_data_t *data	 = static_data->data;
	uintmax_t		oldbytes = data->bytes;
	char			buf[JU_STR_SIZE];

	if (!static_data->cleanup) static_data->cleanup = netspeed_cleanup;

	SYSFS_FD_OR_SEEK({ ERRRET(out); },
			 data->fd,
			 SYS_CLASS,
			 interface,
			 property);

	EREAD({ ERRRET(out); }, _unused, data->fd, WITH_LEN(buf));
	SCANF({ ERRRET(out); }, 1, sscanf, buf, "%ju", &data->bytes);

	if (!oldbytes) ERRRET(out);

	fmt_human(out, (data->bytes - oldbytes) * 1000 / interval);
}
