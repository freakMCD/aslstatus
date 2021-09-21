#ifndef NETSPEED_H
#define NETSPEED_H

#include <stdio.h>
#include <inttypes.h>

struct netspeed_data {
	int fd;
	uintmax_t bytes;
};
#	define NETSPEED_STATIC_SIZE 

#endif /* NETSPEED_H */
