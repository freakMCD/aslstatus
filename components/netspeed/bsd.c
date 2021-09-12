#include <err.h>
#include <stdio.h>
#include <limits.h>

#include <net/if.h>
#include <string.h>
#include <ifaddrs.h>
#include <sys/types.h>
#include <sys/socket.h>

#include "../../util.h"

void
netspeed_rx(char *	 out,
	    const char * interface,
	    unsigned int interval,
	    void *	 static_ptr)
{
	int		if_ok = 0;
	struct ifaddrs *ifa;
	struct if_data *ifd;
	struct ifaddrs *ifal;
	uintmax_t	oldrxbytes;
	uintmax_t *	rxbytes = static_ptr;

	oldrxbytes		= *rxbytes;

	if (getifaddrs(&ifal) == -1) {
		warnx("getifaddrs failed");
		ERRRET(out);
	}
	*rxbytes = 0;
	for (ifa = ifal; ifa; ifa = ifa->ifa_next) {
		if (!strcmp(ifa->ifa_name, interface)
		    && (ifd = (struct if_data *)ifa->ifa_data)) {
			*rxbytes += ifd->ifi_ibytes, if_ok = !0;
		}
	}
	freeifaddrs(ifal);
	if (!if_ok) {
		warnx("reading 'if_data' failed");
		ERRRET(out);
	}
	if (oldrxbytes == 0) { ERRRET(out); }

	fmt_human(out, (*rxbytes - oldrxbytes) * 1000 / interval);
}

void
netspeed_tx(char *	 out,
	    const char * interface,
	    unsigned int interval,
	    void *	 static_ptr)
{
	int		if_ok = 0;
	struct ifaddrs *ifa;
	struct if_data *ifd;
	struct ifaddrs *ifal;
	uintmax_t	oldtxbytes;
	uintmax_t *	txbytes = static_ptr;

	oldtxbytes		= *txbytes;

	if (getifaddrs(&ifal) == -1) {
		warnx("getifaddrs failed");
		ERRRET(out);
	}
	*txbytes = 0;
	for (ifa = ifal; ifa; ifa = ifa->ifa_next) {
		if (!strcmp(ifa->ifa_name, interface)
		    && (ifd = (struct if_data *)ifa->ifa_data)) {
			*txbytes += ifd->ifi_obytes, if_ok = 1;
		}
	}
	freeifaddrs(ifal);
	if (!if_ok) {
		warnx("reading 'if_data' failed");
		ERRRET(out);
	}
	if (oldtxbytes == 0) { ERRRET(out); }

	fmt_human(out, (*txbytes - oldtxbytes) * 1000 / interval);
}
