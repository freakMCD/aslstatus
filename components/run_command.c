/* See LICENSE file for copyright and license details. */
#include <err.h>
#include <stdio.h>
#include <string.h>

#include "../lib/util.h"

void
run_command(char *		  buf,
	    const char *	  cmd,
	    unsigned int __unused _i,
	    void __unused *_p)
{
	char *p;
	FILE *fp;

	if (!(fp = popen(cmd, "re"))) {
		warn("popen(" QUOTED("%s") ", " QUOTED("re") ")", cmd);
		ERRRET(buf);
	}

	p = fgets(buf, BUFF_SZ - 1, fp);

	EFUNC(
	    0 > (signed),
	    { ERRRET(buf); },
	    _unused,
	    pclose,
	    fp);

	if (!p) ERRRET(buf);
	if (!!(p = strrchr(buf, '\n'))) *p = '\0';
}
