#include "brightness.h"	 /* struct brightness_data */
#include "../lib/util.h" /* see functions description here */
#include "../aslstatus.h"

#define SYSFS_CLASS "/sys/class/backlight"

static void brightness_cleanup(void *ptr);

void
brightness(char		*out,
	   const char	      *device,
	   uint32_t __unused _i,
	   static_data_t	 *static_data)
{
	/*
	 * `static_data_t` used to have static data per thread
	 *
	 * `static` keyword will save data staticly for function,
	 * but not for thread
	 */
	struct brightness_data *data = static_data->data;

	char	buf[INT_STR_SIZE]; /* buffer for reading data from file */
	uint8_t err    = 0;	   /* is error occurred */
	int	max_fd = -1;	   /* fd of max_brightness */

	__typeof__(data->max_brightness) brightness;

	/* add cleanup function */
	if (!static_data->cleanup) static_data->cleanup = brightness_cleanup;

	if (!data->max_brightness) {
		/* open file */
		if ((max_fd = sysfs_fd(SYSFS_CLASS, device, "max_brightness"))
		    == -1)
			ERRRET(out);

		/* read data to buffer */
		if (!eread(max_fd, WITH_LEN(buf))) err = !0;

		/* close file after reading */
		eclose(max_fd);

		if (err) ERRRET(out);

		if (!esscanf(1, buf, "%u", &data->max_brightness)) ERRRET(out);
	}

	/* open `brightness` file */
	if (!sysfs_fd_or_rewind(&data->fd, SYSFS_CLASS, device, "brightness"))
		ERRRET(out);

	if (!eread(data->fd, WITH_LEN(buf))) ERRRET(out);

	if (!esscanf(1, buf, "%u", &brightness)) ERRRET(out);

	bprintf(out,
		"%" PRIperc,
		(percent_t)(100 * brightness / data->max_brightness));

	/*
	 * NOTE: `brightness` file not closed
	 * it will be closed at cleanup
	 */
}

static inline void
brightness_cleanup(void *ptr)
{
	/* close `brightness` fd at exit */
	eclose(((struct brightness_data *)ptr)->fd);
}
