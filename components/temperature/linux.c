/* See LICENSE file for copyright and license details. */
#include <err.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <dirent.h>
#include <limits.h>
#include <unistd.h>

#include "../../util.h"

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
	uintmax_t temp;

	DIR *	       d;
	struct dirent *dp;

	uint8_t i, found = 0;
	size_t	readed;
	int	name_fd;
	char	name[MAX_NAME];
	FILE ** fptr = static_ptr;

	if (!!*fptr) {
		SEEK_0(*fptr, { ERRRET(out); });
		goto get_temp;
	}

	if (!!device) {
		if (sysfs_fptr(fptr, SYS_CLASS, device, TEMP_FILE))
			ERRRET(out);
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

		readed = read(name_fd, name, sizeof(name));
		close(name_fd);

		if ((signed)readed == -1) {
			warn("read fd:%d (%s/%s/name)",
			     name_fd,
			     SYS_CLASS,
			     dp->d_type);
			continue;
		} else {
			/* remove last new line char */
			if (name[readed - 1] == '\n') --readed;
			name[readed] = '\0';
		}

		for (i = 0; i < LEN(GOOD_NAMES); i++) {
			if (!strncmp(name, GOOD_NAMES[i], readed)) {
				if (sysfs_fptr(fptr,
					       SYS_CLASS,
					       dp->d_name,
					       TEMP_FILE))
					ERRRET(out);
				found = !0;
				goto end_loop;
			}
		}
	}
end_loop:
	closedir(d);

	if (!!found) {
	get_temp:
		if (fscanf(*fptr, "%ju", &temp) != 1) ERRRET(out);
		bprintf(out, "%ju", temp / 1000);
	}
}
