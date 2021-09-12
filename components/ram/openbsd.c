#include <stdio.h>

#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/sysctl.h>

#include "../../util.h"

#define LOG1024			 10
#define pagetok(size, pageshift) (size_t)(size << (pageshift - LOG1024))

static inline int
load_uvmexp(struct uvmexp *uvmexp)
{
	size_t size;
	int    uvmexp_mib[] = { CTL_VM, VM_UVMEXP };

	size		    = sizeof(*uvmexp);

	if (sysctl(uvmexp_mib, 2, uvmexp, &size, NULL, 0) >= 0) return 1;

	return 0;
}

void
ram_free(char *	    out,
	 const char __unused * _a,
	 unsigned int __unused _i,
	 void __unused *_p)
{
	int	      free_pages;
	struct uvmexp uvmexp;

	if (load_uvmexp(&uvmexp)) {
		free_pages = uvmexp.npages - uvmexp.active;
		fmt_human(out,
			  pagetok(free_pages, uvmexp.pageshift) * 1024,
			  1024);
	}

	ERRRET(out);
}

void
ram_perc(char *	    out,
	 const char __unused * _a,
	 unsigned int __unused _i,
	 void __unused *_p)
{
	struct uvmexp uvmexp;
	int	      percent;

	if (load_uvmexp(&uvmexp)) {
		percent = uvmexp.active * 100 / uvmexp.npages;
		bprintf(out, "%d", percent);
	}

	ERRRET(out);
}

void
ram_total(char *     out,
	  const char __unused * _a,
	  unsigned int __unused _i,
	  void __unused *_p)
{
	struct uvmexp uvmexp;

	if (load_uvmexp(&uvmexp))
		fmt_human(out,
			  pagetok(uvmexp.npages, uvmexp.pageshift) * 1024,
			  1024);

	ERRRET(out);
}

void
ram_used(char *	    out,
	 const char __unused * _a,
	 unsigned int __unused _i,
	 void __unused *_p)
{
	struct uvmexp uvmexp;

	if (load_uvmexp(&uvmexp))
		fmt_human(out,
			  pagetok(uvmexp.active, uvmexp.pageshift) * 1024,
			  1024);

	ERRRET(out);
}
