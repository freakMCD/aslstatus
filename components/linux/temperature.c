/* See LICENSE file for copyright and license details. */
#include <err.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <dirent.h>
#include <limits.h>
#include <unistd.h>

#include "../../lib/util.h"

#define MAX_NAME  16
#define TEMP_FILE "temp1_input"
#define SYS_CLASS "/sys/class/hwmon"

static const char *GOOD_NAMES[] = {
	"coretemp",
	"acpitz",
	"k10temp",
	"fam15h_power",
};

void
temp(char *out, const char *device, unsigned int __unused _i, void *static_ptr)
{
	DIR *	       d;
	struct dirent *dp;

	uint8_t i, found = 0;
	size_t	readed;
	int	name_fd;
	char	name[MAX_NAME];
	char	buf[JU_STR_SIZE + 3 /* zeros at the end */];
	int *	fd = static_ptr;

	if (*fd > 0) {
		SEEK_0({ ERRRET(out); }, *fd);
		goto get_temp;
	}

	if (!!device) {
		SYSFS_FD_OR_SEEK({ ERRRET(out); },
				 *fd,
				 SYS_CLASS,
				 device,
				 TEMP_FILE);
		goto get_temp;
	}

	if (!(d = opendir(SYS_CLASS))) {
		warn("opendir(%s)", SYS_CLASS);
		ERRRET(out);
	}

	while ((dp = readdir(d))) {
		if (!strcmp(dp->d_name, ".") || !strcmp(dp->d_name, ".."))
			continue;

		if ((name_fd = sysfs_fd(SYS_CLASS, dp->d_name, "name")) == -1)
			continue;

		EREAD(
		    {
			    close(name_fd);
			    continue;
		    },
		    readed,
		    name_fd,
		    WITH_LEN(name));

		/* remove last new line char */
		name[--readed] = '\0';

		for (i = 0; i < LEN(GOOD_NAMES); i++) {
			if (!strncmp(name, GOOD_NAMES[i], readed)) {
				SYSFS_FD_OR_SEEK({ ERRRET(out); },
						 *fd,
						 SYS_CLASS,
						 dp->d_name,
						 TEMP_FILE);
				found = !0;
				goto end_loop;
			}
		}
	}
end_loop:
	closedir(d);

	if (!!found) {
	get_temp:
		EREAD({ ERRRET(out); }, readed, *fd, WITH_LEN(buf));
		if (readed > 4)
			readed -= 4; /* 3 zeros and '\n' at the end */
		else
			buf[(readed = 1) - 1] = '0';

		buf[readed] = '\0';
		bprintf(out, "%s", buf);
	}
}
