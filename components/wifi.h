#ifndef WIFI_H
#define WIFI_H

#include <linux/wireless.h>

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

#endif /* WIFI_H */
