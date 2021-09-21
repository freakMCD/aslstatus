1i\
#include <stdio.h>                                                            \
#include "../lib/util.h"                                                      \
#define FUNC_ARGS (char __unused _o, const char __unused* _a,               \\\
		   uint32_t __unused _i, void __unused* _p)                   \
#if defined(USE_ALSA)                                                         \
#	define VOLUME "alsa"                                                  \
#elif defined(USE_PULSE)                                                      \
#	define VOLUME "pulse"                                                 \
#else                                                                         \
#	define VOLUME "default"                                               \
#endif

/^\(void\s\+[^ 	]\+\s\+FUNC_ARGS\);$/ {

	s//\1 {puts(__func__);}/

	/vol_perc/ {
		s!\(puts\)([^)]\+)!\1("file:volume/"VOLUME".c")!
	}

	/\(swap\|ram\)/ {
		s!}$!puts("file:../lib/meminfo.c");}!
	}

	p
}
