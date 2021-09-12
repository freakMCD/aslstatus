#include <stdio.h>
#include <unistd.h>
#include <sys/sysctl.h>
#include <sys/vmmeter.h>
#include <vm/vm_param.h>

#include "../../util.h"

void
ram_free(char *	    out,
	 const char __unused * _a,
	 unsigned int __unused _i,
	 void __unused *_p)
{
	size_t	       len;
	struct vmtotal vm_stats;
	int	       mib[] = { CTL_VM, VM_TOTAL };

	len		     = sizeof(struct vmtotal);
	if (sysctl(mib, 2, &vm_stats, &len, NULL, 0) == -1 || !len)
		ERRRET(out);

	fmt_human(out, vm_stats.t_free * getpagesize(), 1024);
}

void
ram_total(char *     out,
	  const char __unused * _a,
	  unsigned int __unused _i,
	  void __unused *_p)
{
	size_t	 len;
	long int npages;

	len = sizeof(npages);
	if (sysctlbyname("vm.stats.vm.v_page_count", &npages, &len, NULL, 0)
		== -1
	    || !len)
		ERRRET(out);

	fmt_human(out, npages * getpagesize(), 1024);
}

void
ram_perc(char *	    out,
	 const char __unused * _a,
	 unsigned int __unused _i,
	 void __unused *_p)
{
	size_t	 len;
	long int npages;
	long int active;

	len = sizeof(npages);
	if (sysctlbyname("vm.stats.vm.v_page_count", &npages, &len, NULL, 0)
		== -1
	    || !len)
		ERRRET(out);

	if (sysctlbyname("vm.stats.vm.v_active_count", &active, &len, NULL, 0)
		== -1
	    || !len)
		ERRRET(out);

	bprintf(out, "%d", active * 100 / npages);
}

void
ram_used(char *	    out,
	 const char __unused * _a,
	 unsigned int __unused _i,
	 void __unused *_p)
{
	size_t	 len;
	long int active;

	len = sizeof(active);
	if (sysctlbyname("vm.stats.vm.v_active_count", &active, &len, NULL, 0)
		== -1
	    || !len)
		ERRRET(out);

	fmt_human(out, active * getpagesize(), 1024);
}
