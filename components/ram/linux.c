#include <stdio.h>
#include <stdlib.h>

#include "../../util.h"
#include "../libmeminfo.h"

static inline memory_t get_used(const struct meminfo* info);

void
ram_free(char*	    out,
	 const char __unused*  _a,
	 unsigned int __unused _i,
	 void*		       static_ptr)
{
	FILE**	       fptr = static_ptr;
	struct meminfo info = MEMINFO_INIT;

	if (MEMINFO_FPTR(fptr)) ERRRET(out);

	if (!get_meminfo(*fptr, &info)) ERRRET(out);

	fmt_human(
	    out,
	    (info.available ? MIN(info.available, info.total) : info.free)
		* 1024);
}

void
ram_perc(char*	    out,
	 const char __unused*  _a,
	 unsigned int __unused _i,
	 void*		       static_ptr)
{
	FILE**	       fptr = static_ptr;
	struct meminfo info = MEMINFO_INIT;

	if (MEMINFO_FPTR(fptr)) ERRRET(out);

	if (!get_meminfo(*fptr, &info)) ERRRET(out);

	if (!info.total) ERRRET(out);

	bprintf(out, "%hhu", (uint8_t)(100 * get_used(&info) / info.total));
}

void
ram_total(char*	     out,
	  const char __unused*	_a,
	  unsigned int __unused _i,
	  void*			static_ptr)
{
	memory_t total;
	FILE**	 fptr = static_ptr;

	if (MEMINFO_FPTR(fptr)) ERRRET(out);

	if (!read_meminfo(*fptr, "MemTotal", &total)) ERRRET(out);

	fmt_human(out, total * 1024);
}

void
ram_used(char*	    out,
	 const char __unused*  _a,
	 unsigned int __unused _i,
	 void*		       static_ptr)
{
	FILE**	       fptr = static_ptr;
	struct meminfo info = MEMINFO_INIT;

	if (MEMINFO_FPTR(fptr)) ERRRET(out);

	if (!get_meminfo(*fptr, &info)) ERRRET(out);

	fmt_human(out, get_used(&info) * 1024);
}

static inline memory_t
get_used(const struct meminfo* info)
{
	/*
	 * see procps(free):
	 * https://gitlab.com/procps-ng/procps/-/blob/master/proc/sysinfo.c
	 *
	 * and htop:
	 * https://github.com/htop-dev/htop/blob/master/linux/LinuxProcessList.c
	 */

	const memory_t diff =
	    info->free
	    + (info->cached + info->reclaimable - info->shared
	       /*
		* Adjustments:
		*  - Shmem in part of Cached
		*  https://lore.kernel.org/patchwork/patch/648763/
		*/
	       )
	    + info->buffers;

	return (info->total >= diff) ? info->total - diff
				     : info->total - info->free;
}
