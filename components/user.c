/* See LICENSE file for copyright and license details. */
#include <pwd.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

#include "../util.h"

void
gid(char *out)
{ bprintf(out, "%d", getgid()); }

void
uid(char *out)
{ bprintf(out, "%d", geteuid()); }

void
username(char *out)
{
	struct passwd *pw;

	if (!(pw = getpwuid(geteuid()))) {
		warn("getpwuid '%d':", geteuid());
		ERRRET(out);
	}

	bprintf(out, "%s", pw->pw_name);
}
