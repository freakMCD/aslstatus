#include <err.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <ifaddrs.h>

#include <sys/ioctl.h>
#include <sys/socket.h>

#include <stdlib.h>
#include <net/if.h>
#include <sys/types.h>
#include <sys/select.h> /* before <sys/ieee80211_ioctl.h> for NBBY */
#include <net/if_media.h>
#include <net80211/ieee80211.h>
#include <net80211/ieee80211_ioctl.h>

#include "../wifi.h"
#include "../../lib/util.h"

static inline int
load_ieee80211_nodereq(const char *interface, struct ieee80211_nodereq *nr)
{
	struct ieee80211_bssid bssid;
	int		       sockfd;
	uint8_t		       zero_bssid[IEEE80211_ADDR_LEN];

	memset(&bssid, 0, sizeof(bssid));
	memset(nr, 0, sizeof(struct ieee80211_nodereq));
	if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		warn("socket 'AF_INET'");
		return 0;
	}
	strlcpy(bssid.i_name, interface, sizeof(bssid.i_name));
	if ((ioctl(sockfd, SIOCG80211BSSID, &bssid)) < 0) {
		warn("ioctl 'SIOCG80211BSSID'");
		close(sockfd);
		return 0;
	}
	memset(&zero_bssid, 0, sizeof(zero_bssid));
	if (memcmp(bssid.i_bssid, zero_bssid, IEEE80211_ADDR_LEN) == 0) {
		close(sockfd);
		return 0;
	}
	strlcpy(nr->nr_ifname, interface, sizeof(nr->nr_ifname));
	memcpy(&nr->nr_macaddr, bssid.i_bssid, sizeof(nr->nr_macaddr));
	if ((ioctl(sockfd, SIOCG80211NODE, nr)) < 0 && nr->nr_rssi) {
		warn("ioctl 'SIOCG80211NODE'");
		close(sockfd);
		return 0;
	}

	return close(sockfd), 1;
}

void
wifi_perc(char *		out,
	  const char *		interface,
	  unsigned int __unused _i,
	  void __unused *_p)
{
	struct ieee80211_nodereq nr;
	int			 q;

	if (load_ieee80211_nodereq(interface, &nr)) {
		if (nr.nr_max_rssi)
			q = IEEE80211_NODEREQ_RSSI(&nr);
		else
			q = RSSI_TO_PERC(nr.nr_rssi);
		bprintf(out, "%d", q);
		return;
	}
	ERRRET(out);
}

void
wifi_essid(char *		 out,
	   const char *		 interface,
	   unsigned int __unused _i,
	   void __unused *_p)
{
	struct ieee80211_nodereq nr;

	if (load_ieee80211_nodereq(interface, &nr)) {
		bprintf(out, "%s", nr.nr_nwid);
		return;
	}

	ERRRET(out);
}
