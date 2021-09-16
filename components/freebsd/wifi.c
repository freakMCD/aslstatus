#include <err.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <ifaddrs.h>

#include <sys/ioctl.h>
#include <sys/socket.h>

#include <net/if.h>
#include <net80211/ieee80211_ioctl.h>

#include "../wifi.h"
#include "../../lib/util.h"

static inline int
load_ieee80211req(
    int sock, const char *interface, void *data, int type, size_t *len)
{
	struct ieee80211req ireq;
	memset(&ireq, 0, sizeof(ireq));
	ireq.i_type = type;
	ireq.i_data = (caddr_t)data;
	ireq.i_len  = *len;

	strlcpy(ireq.i_name, interface, sizeof(ireq.i_name));
	if (ioctl(sock, SIOCG80211, &ireq) < 0) {
		warn("ioctl: 'SIOCG80211': %d", type);
		return 0;
	}

	*len = ireq.i_len;
	return 1;
}

void
wifi_perc(char *		out,
	  const char *		interface,
	  unsigned int __unused _i,
	  void __unused *_p)
{
	union {
		struct ieee80211req_sta_req sta;
		uint8_t			    buf[24 * 1024];
	} info;
	size_t	len;
	int	sockfd;
	int	rssi_dbm;
	uint8_t bssid[IEEE80211_ADDR_LEN];

	if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		warn("socket 'AF_INET'");
		ERRRET(out);
	}

	/* Retreive MAC address of interface */
	len = IEEE80211_ADDR_LEN;
	if (load_ieee80211req(sockfd,
			      interface,
			      &bssid,
			      IEEE80211_IOC_BSSID,
			      &len)) {
		/* Retrieve info on station with above BSSID */
		memset(&info, 0, sizeof(info));
		memcpy(info.sta.is_u.macaddr, bssid, sizeof(bssid));

		len = sizeof(info);
		if (load_ieee80211req(sockfd,
				      interface,
				      &info,
				      IEEE80211_IOC_STA_INFO,
				      &len)) {
			rssi_dbm = info.sta.info[0].isi_noise
				 + info.sta.info[0].isi_rssi / 2;

			bprintf(out, "%d", RSSI_TO_PERC(rssi_dbm));
		}
	}
	close(sockfd);
}

void
wifi_essid(char *		 out,
	   const char *		 interface,
	   unsigned int __unused _i,
	   void __unused *_p)
{
	size_t len;
	int    sockfd;
	char   ssid[IEEE80211_NWID_LEN + 1];

	if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		warn("socket 'AF_INET'");
		ERRRET(out);
	}

	len = sizeof(ssid);
	memset(&ssid, 0, len);
	if (load_ieee80211req(sockfd,
			      interface,
			      &ssid,
			      IEEE80211_IOC_SSID,
			      &len)) {
		if (len < sizeof(ssid))
			len += 1;
		else
			len = sizeof(ssid);

		ssid[len - 1] = '\0';
		bprintf(out, "%s", ssid);
	}
	close(sockfd);
}
