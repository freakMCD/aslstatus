#ifndef NETSPEED_H
#define NETSPEED_H

#include <stdio.h>
#include <inttypes.h>

#include "../../os.h"

#if LINUX
struct netspeed_data_t {
	FILE*	  fptr;
	uintmax_t bytes;
};
#	define NETSPEED_STATIC_SIZE sizeof(struct netspeed_data_t)

#else

#	define NETSPEED_STATIC_SIZE sizeof(uintmax_t)
#endif /* LINUX */

#endif /* NETSPEED_H */
