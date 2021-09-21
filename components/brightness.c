#include "../lib/util.h"
#include "../aslstatus.h"

#define SYSFS_CLASS "/sys/class/backlight"

static void brightness_cleanup(void *ptr);

struct brightness_data {
	int	 fd;
	uint32_t max_brightness;
};

void
brightness(char *	     out,
	   const char *	     device,
	   uint32_t __unused _i,
	   static_data_t *   static_data)
{
	struct brightness_data *data = static_data->data;

	char buf[INT_STR_SIZE];
	int  max_fd = -1;

	__typeof__(data->max_brightness) brightness;

	if (!static_data->cleanup) static_data->cleanup = brightness_cleanup;

	if (!data->max_brightness) {
		if ((max_fd = sysfs_fd(SYSFS_CLASS, device, "max_brightness"))
		    == -1)
			ERRRET(out);
		if (!eread(max_fd, WITH_LEN(buf))) goto err;

		eclose(max_fd);

		if (!esscanf(1, buf, "%u", &data->max_brightness)) ERRRET(out);
	}

	if (!sysfs_fd_or_rewind(&data->fd, SYSFS_CLASS, device, "brightness"))
		ERRRET(out);

	if (!eread(data->fd, WITH_LEN(buf))) ERRRET(out);
	if (!esscanf(1, buf, "%u", &brightness)) ERRRET(out);

	bprintf(out,
		"%hhu",
		(uint8_t)(100 * brightness / data->max_brightness));
	return;

err:
	if (max_fd != -1) eclose(max_fd);
	ERRRET(out);
}

static inline void
brightness_cleanup(void *ptr)
{
	eclose(((struct brightness_data *)ptr)->fd);
}
