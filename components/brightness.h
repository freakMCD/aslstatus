#ifndef BRIGHTNESS_H
#define BRIGHTNESS_H

#include <stdint.h>

struct brightness_data {
	int	 fd;
	uint32_t max_brightness;
};

#endif /* BRIGHTNESS_H */
