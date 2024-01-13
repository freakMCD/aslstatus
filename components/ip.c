/* See LICENSE file for copyright and license details. */
#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ifaddrs.h>

#if defined(__OpenBSD__)
#	include <sys/types.h>
#	include <sys/socket.h>
#elif defined(__FreeBSD__)
#	include <netinet/in.h>
#	include <sys/socket.h>
#endif

#include "../lib/util.h"

static int check=1;

static inline void
ip(char *out, const char *interface, sa_family_t sa_family)
{
	struct ifaddrs *ifa, *ifaddr;

	if (getifaddrs(&ifaddr) < 0) {
		warn("getifaddrs");
		ERRRET(out);
	}

	for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
		if (!ifa->ifa_addr) { continue; }
		if (!strcmp(ifa->ifa_name, interface)
		    && (ifa->ifa_addr->sa_family == sa_family)) {
			freeifaddrs(ifaddr);
			bprintf(out, "^c#a9b665^󰈀 up");
            if (check == 1) {
                system("notify-send -t 3000 'Network' 'Connected'");
                check=0;
            }
			return;
		}
	}
	freeifaddrs(ifaddr);
    bprintf(out, "^c#ea6962^󰈀 down");
    if (check == 0) {
        system("notify-send -t 3000 'Network' 'Disconnected'");
        check=1;
    }
}

void
ipv4(char *out, const char *interface, uint32_t __unused _i, void __unused *_p)
{
	ip(out, interface, AF_INET);
}

void
ipv6(char *out, const char *interface, uint32_t __unused _i, void __unused *_p)
{
	ip(out, interface, AF_INET6);
}
