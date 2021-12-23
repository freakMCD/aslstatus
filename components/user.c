/* See LICENSE file for copyright and license details. */
#include <err.h>
#include <pwd.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

#include "../lib/util.h"

void
gid(char		 *out,
    const char __unused *_a,
    uint32_t __unused	 _i,
    void __unused	  *_p)
{
	bprintf(out, "%u", getgid());
}

void
uid(char		 *out,
    const char __unused *_a,
    uint32_t __unused	 _i,
    void __unused	  *_p)
{
	bprintf(out, "%u", getuid());
}

void
username(char		      *out,
	 const char __unused *_a,
	 uint32_t __unused    _i,
	 void __unused       *_p)
{
	struct passwd *pw;

	uid_t uid = getuid();

	if (!(pw = getpwuid(uid))) {
		warn("getpwuid(%u)", uid);
		ERRRET(out);
	}

	bprintf(out, "%s", pw->pw_name);
}
