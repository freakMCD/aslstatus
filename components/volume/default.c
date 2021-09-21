#include <err.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#include <sys/ioctl.h>
#include <sys/stat.h>
#include "../../lib/util.h"
#include "../../components_config.h"

#ifndef VOLUME_SYM
#	define VOLUME_SYM ""
#endif

#ifndef VOLUME_PERCENT
#	define VOLUME_PERCENT " %"
#endif

#include <sys/soundcard.h>

void
vol_perc(char *out, const char *card, uint32_t __unused _i, void __unused *_p)
{
	size_t i;
	int    v, afd, devmask;
	char * vnames[] = SOUND_DEVICE_NAMES;

	if ((afd = open(card, O_RDONLY | O_NONBLOCK)) < 0) {
		warn("open '%s'", card);
		ERRRET(out);
	}

	if (ioctl(afd, (int)SOUND_MIXER_READ_DEVMASK, &devmask) < 0) {
		warn("ioctl 'SOUND_MIXER_READ_DEVMASK'");
		close(afd);
		ERRRET(out);
	}
	for (i = 0; i < LEN(vnames); i++) {
		if (devmask & (1 << i) && !strcmp("vol", vnames[i])) {
			if (ioctl(afd, MIXER_READ(i), &v) < 0) {
				warn("ioctl 'MIXER_READ(%ld)'", i);
				close(afd);
				ERRRET(out);
			}
		}
	}

	close(afd);

	bprintf(out, "%s%3d%s", VOLUME_SYM, v & 0xff, VOLUME_PERCENT);
}
