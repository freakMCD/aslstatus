/* See LICENSE file for copyright and license details. */
#include <err.h>
#include <stdio.h>
#include <string.h>

#include "../lib/util.h"

void
run_command(char *	      out,
	    const char *      cmd,
	    uint32_t __unused _i,
	    void __unused *_p)
{
	char *p;
	FILE *fp;

	if (!(fp = popen(cmd, "re"))) {
		warn("popen(" QUOTED("%s") ", " QUOTED("re") ")", cmd);
		ERRRET(out);
	}

	p = fgets(out, BUFF_SZ - 1, fp);

	if (pclose(fp) == -1) ERRRET(out);

	if (!p) ERRRET(out);
	if (!!(p = strrchr(out, '\n'))) *p = '\0';
}
