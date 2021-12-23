#include <err.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>

#include <sys/un.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>

#if USE_X
#	include <xcb/xcb.h>
#endif

#include "../lib/util.h"
#include "../aslstatus.h"
#include "../components_config.h"

#ifndef BSPWM_FOCUSED_RESET
#	define BSPWM_FOCUSED_RESET "%{F-}%{B-}"
#endif

#ifndef BSPWM_FOCUSED_FG
#	define BSPWM_FOCUSED_FG "#0ff"
#endif

#ifndef BSPWM_FOCUSED_BG
#	define BSPWM_FOCUSED_BG "-"
#endif

#ifndef BSPWM_DELIM
#	define BSPWM_DELIM ' '
#endif

#define FAILURE_MESSAGE '\x07'
#define SOCKET_ENV_VAR	"BSPWM_SOCKET"
#define SOCKET_PATH_TPL "/tmp/bspwm%s_%i_%i-socket"

#define FOCUSED_PREFIX "%{F" BSPWM_FOCUSED_FG "}%{B" BSPWM_FOCUSED_BG "}"

#define WITH_LEN(S) S, LEN(S)
#define DESKTOP_END(D)                                                        \
	(D) = !0;                                                             \
	break

/*
 * add delimiter if not first char
 * O	out
 * S	out size
 * I	index
 */
#define ADD_DELIM(O, S, I)                                                    \
	do {                                                                  \
		if (((I) + 1) > S) ERRRET(O);                                 \
		(O)[(I)] = BSPWM_DELIM;                                       \
		((I) += !!(I));                                               \
	} while (0)
/* add FOCUSED_RESET if end of focused desktop
 * F	`is_focused` identifier */
#define END_FOCUS(F, O, S, I)                                                 \
	do {                                                                  \
		if (!!(F)) {                                                  \
			(F) = !(F);                                           \
			size_t _len;                                          \
			if ((_len = sizeof(BSPWM_FOCUSED_RESET) - 1) > (S))   \
				ERRRET(O);                                    \
			memcpy((O) + (I), BSPWM_FOCUSED_RESET, _len);         \
			(I) += _len;                                          \
		}                                                             \
	} while (0)

#if USE_X
static void parse_event(const char *, size_t, char *, size_t);

void
bspwm_ws(char		      *out,
	 const char __unused *_a,
	 uint32_t __unused    _i,
	 static_data_t       *static_data)
{
	char		     *sp, rsp[BUFSIZ];
	int		   nb, *fd = static_data->data;
	struct sockaddr_un sock_address;

	if (!static_data->cleanup) static_data->cleanup = fd_cleanup;

	if (!!*fd) goto run;

	sock_address.sun_family = AF_UNIX;

	if ((sp = getenv(SOCKET_ENV_VAR))) {
		snprintf(sock_address.sun_path,
			 sizeof(sock_address.sun_path),
			 "%s",
			 sp);
	} else {
		int   dn = 0, sn = 0;
		char *host = NULL;
		if (xcb_parse_display(NULL, &host, &dn, &sn))
			snprintf(sock_address.sun_path,
				 sizeof(sock_address.sun_path),
				 SOCKET_PATH_TPL,
				 host,
				 dn,
				 sn);
		else
			ERRRET(out);
		free(host);
	}

	if ((*fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
		warn("Failed to create socket");
		ERRRET(out);
	}
	if (connect(*fd,
		    (struct sockaddr *)&sock_address,
		    sizeof(sock_address))
	    == -1) {
		warn("Failed to connect to socket");
		ERRRET(out);
	}

	if (send(*fd, WITH_LEN("subscribe"), 0) == -1) {
		warn("Failed to subscribe to socket");
		ERRRET(out);
	}

	struct pollfd poll_fd = { *fd, POLLIN, 0 };

run:
	if (poll(&poll_fd, 1, -1) > 0)
		if (poll_fd.revents & POLLIN) {
			if (SAFE_ASSIGN(nb, recv(*fd, rsp, sizeof(rsp) - 1, 0))
			    > 0) {
				if (*rsp == FAILURE_MESSAGE)
					warn("%s", rsp + 1);
				else
					parse_event(rsp,
						    (size_t)nb,
						    out,
						    BUFF_SZ);
			} else {
				ERRRET(out);
			}
		}
}

static void
parse_event(const char *event, size_t size, char *out, size_t out_size)
{
	size_t	i, j;
	uint8_t is_desktop = 0, is_focused = 0;

	for (i = j = 0; i < size && j < out_size; i++) {
		if (event[i] == ':') {
			switch (event[i + 1]) {
			case 'o':
#	if defined(BSPWM_SHOW_VACANT_TAGS)
			case 'f':
#	endif /* BSPWM_SHOW_VACANT_TAGS */
				END_FOCUS(is_focused, out, out_size, j);

				ADD_DELIM(out, out_size, j);

				DESKTOP_END(is_desktop);

			case 'F':
			case 'O':
				is_focused = !0;

				ADD_DELIM(out, out_size, j);

				memcpy(out + j, WITH_LEN(FOCUSED_PREFIX));
				j += sizeof(FOCUSED_PREFIX) - 1;

				DESKTOP_END(is_desktop);

			default:
				is_desktop = 0;

				END_FOCUS(is_focused, out, out_size, j);
			}
			i++;
			continue;
		} else if (!is_desktop) {
			continue;
		}
		out[j++] = event[i];
	}
	out[j] = '\0';
}
#endif /* USE_X */
