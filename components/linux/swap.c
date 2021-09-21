#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <sys/sysinfo.h>

#include "../../lib/util.h"
#include "../../aslstatus.h"
#include "../../lib/meminfo.h"

#define DEF_SWAP(STRUCT, STATIC, OUT)                                         \
	int*		    fd	   = (STATIC)->data;                          \
	struct meminfo_swap STRUCT = MEMINFO_INIT_SWAP;                       \
	if (!(STATIC)->cleanup) (STATIC)->cleanup = fd_cleanup;               \
	if (!MEMINFO_FD(fd)) ERRRET(OUT);                                     \
	if (!get_meminfo_swap(*fd, &STRUCT)) ERRRET(OUT)

void
swap_free(char*	     out,
	  const char __unused* _a,
	  uint32_t __unused    _i,
	  static_data_t*       static_data)
{
	DEF_SWAP(info, static_data, out);

	fmt_human(out, info.free * 1024);
}

void
swap_perc(char*	     out,
	  const char __unused* _a,
	  uint32_t __unused    _i,
	  static_data_t*       static_data)
{
	DEF_SWAP(info, static_data, out);

	bprintf(out,
		"%" PRIperc,
		(percent_t)(100 * (info.total - info.free - info.cached)
			    / info.total));
}

void
swap_total(char*      out,
	   const char __unused* _a,
	   uint32_t __unused	_i,
	   void __unused* _p)
{
	struct sysinfo info;
	if (!!sysinfo(&info)) ERRRET(out);

	fmt_human(out, info.totalswap * info.mem_unit);
}

void
swap_used(char*	     out,
	  const char __unused* _a,
	  uint32_t __unused    _i,
	  static_data_t*       static_data)
{
	DEF_SWAP(info, static_data, out);

	fmt_human(out, (info.total - info.free - info.cached) * 1024);
}
