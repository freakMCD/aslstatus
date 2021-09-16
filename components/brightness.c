#include "../lib/util.h"

#define SYSFS_CLASS "/sys/class/backlight"

void
brightness(char *		 out,
	   const char *		 device,
	   unsigned int __unused _i,
	   void *		 static_ptr)
{
	struct {
		int	 fd;
		uint32_t max_brightness;
	} *data	    = static_ptr;

	int  max_fd = -1;
	char buf[INT_STR_SIZE];

	__typeof__(data->max_brightness) brightness;

	if (!data->max_brightness) {
		if ((max_fd = sysfs_fd(SYSFS_CLASS, device, "max_brightness"))
		    == -1)
			ERRRET(out);
		EREAD({ goto err; }, _unused, max_fd, WITH_LEN(buf));
		close(max_fd);

		SCANF({ ERRRET(out); },
		      1,
		      sscanf,
		      buf,
		      "%u",
		      &data->max_brightness);
	}

	SYSFS_FD_OR_SEEK({ ERRRET(out); },
			 data->fd,
			 SYSFS_CLASS,
			 device,
			 "brightness");

	EREAD({ ERRRET(out); }, _unused, data->fd, WITH_LEN(buf));
	SCANF({ ERRRET(out); }, 1, sscanf, buf, "%u", &brightness);

	bprintf(out,
		"%hhu",
		(uint8_t)(100 * brightness / data->max_brightness));
	return;

err:
	if (max_fd != -1) close(max_fd);
	ERRRET(out);
}
