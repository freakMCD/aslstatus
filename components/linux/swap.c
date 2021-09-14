#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "../../lib/util.h"
#include "../../lib/meminfo.h"

void
swap_free(char*	     out,
	  const char __unused*	_a,
	  unsigned int __unused _i,
	  void*			static_ptr)
{
	int*		    fd	 = static_ptr;
	struct meminfo_swap info = MEMINFO_INIT_SWAP;

	MEMINFO_FD({ ERRRET(out); }, *fd);

	if (!get_meminfo_swap(*fd, &info)) ERRRET(out);

	fmt_human(out, info.free * 1024);
}

void
swap_perc(char*	     out,
	  const char __unused*	_a,
	  unsigned int __unused _i,
	  void*			static_ptr)
{
	int*		    fd	 = static_ptr;
	struct meminfo_swap info = MEMINFO_INIT_SWAP;

	MEMINFO_FD({ ERRRET(out); }, *fd);

	if (!get_meminfo_swap(*fd, &info)) ERRRET(out);

	fmt_human(out,
		  100 * (info.total - info.free - info.cached) / info.total);
}

void
swap_total(char*      out,
	   const char __unused*	 _a,
	   unsigned int __unused _i,
	   void*		 static_ptr)
{
	int*		    fd	 = static_ptr;
	struct meminfo_swap info = MEMINFO_INIT_SWAP;

	MEMINFO_FD({ ERRRET(out); }, *fd);

	if (!get_meminfo_swap(*fd, &info)) ERRRET(out);

	fmt_human(out, info.total * 1024);
}

void
swap_used(char*	     out,
	  const char __unused*	_a,
	  unsigned int __unused _i,
	  void*			static_ptr)
{
	int*		    fd	 = static_ptr;
	struct meminfo_swap info = MEMINFO_INIT_SWAP;

	MEMINFO_FD({ ERRRET(out); }, *fd);

	if (!get_meminfo_swap(*fd, &info)) ERRRET(out);

	fmt_human(out, (info.total - info.free - info.cached) * 1024);
}
