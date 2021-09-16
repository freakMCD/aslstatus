#ifndef WIFI_H
#define WIFI_H

#include "../os.h"

#if LINUX
#	include <linux/wireless.h>
struct wifi_common_data {
	int	     sock;
	struct iwreq req;
};
struct wifi_perc_data {
	struct wifi_common_data common;
	struct iw_statistics	stat;
};
struct wifi_essid_data {
	struct wifi_common_data common;
	char			id[IW_ESSID_MAX_SIZE + 1];
};
#	define WIFI_PERC_STATIC_SIZE  sizeof(struct wifi_perc_data)
#	define WIFI_ESSID_STATIC_SIZE sizeof(struct wifi_essid_data)
#else
#	define RSSI_TO_PERC(RSSI)                                            \
		((RSSI) >= -50 ? 100                                          \
			       : ((RSSI) <= -100 ? 0 : (2 * ((RSSI) + 100))))
#	define WIFI_PERC_STATIC_SIZE  0
#	define WIFI_ESSID_STATIC_SIZE 0
#endif

#endif /* WIFI_H */
