#include <err.h>
#include <stdio.h>
#include <limits.h>
#include <stdint.h>

#include "../netspeed.h"
#include "../../lib/util.h"

#define SYS_CLASS "/sys/class/net"

#define STATISTICS(S) "statistics/" #S "_bytes"

static inline void netspeed(char *	 out,
			    const char * interface,
			    unsigned int interval,
			    void *	 static_ptr,
			    const char * property);

void
netspeed_rx(char *	 out,
	    const char * interface,
	    unsigned int interval,
	    void *	 static_ptr)
{
	netspeed(out, interface, interval, static_ptr, STATISTICS(rx));
}

void
netspeed_tx(char *	 out,
	    const char * interface,
	    unsigned int interval,
	    void *	 static_ptr)
{
	netspeed(out, interface, interval, static_ptr, STATISTICS(tx));
}

static inline void
netspeed(char *	      out,
	 const char * interface,
	 unsigned int interval,
	 void *	      static_ptr,
	 const char * property)
{
	struct netspeed_data_t *data	 = static_ptr;
	uintmax_t		oldbytes = data->bytes;
	char			buf[JU_STR_SIZE];

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
