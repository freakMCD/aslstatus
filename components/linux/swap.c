#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <sys/sysinfo.h>

#include "../../lib/util.h"
#include "../../lib/meminfo.h"

#define DEF_SWAP(STRUCT, STATIC, OUT)                                         \
	int*		    fd	   = (STATIC);                                \
	struct meminfo_swap STRUCT = MEMINFO_INIT_SWAP;                       \
	MEMINFO_FD({ ERRRET(OUT); }, *fd);                                    \
	if (!get_meminfo_swap(*fd, &STRUCT)) ERRRET(OUT)

void
swap_free(char*	     out,
	  const char __unused*	_a,
	  unsigned int __unused _i,
	  void*			static_ptr)
{
	DEF_SWAP(info, static_ptr, out);

	fmt_human(out, info.free * 1024);
}

void
swap_perc(char*	     out,
	  const char __unused*	_a,
	  unsigned int __unused _i,
	  void*			static_ptr)
{
	DEF_SWAP(info, static_ptr, out);

	fmt_human(out,
		  100 * (info.total - info.free - info.cached) / info.total);
}

void
swap_total(char*      out,
	   const char __unused*	 _a,
	   unsigned int __unused _i,
	   void __unused* _p)
{
	struct sysinfo info;
	if (!!sysinfo(&info)) ERRRET(out);

	fmt_human(out, info.totalswap * info.mem_unit);
}

void
swap_used(char*	     out,
	  const char __unused*	_a,
	  unsigned int __unused _i,
	  void*			static_ptr)
{
	DEF_SWAP(info, static_ptr, out);

	fmt_human(out, (info.total - info.free - info.cached) * 1024);
}
