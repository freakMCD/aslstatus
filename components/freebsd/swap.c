#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include <stdlib.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <kvm.h>

#include "../../lib/util.h"

static inline int
getswapinfo(struct kvm_swap *swap_info, size_t size)
{
	kvm_t *kd;

	kd = kvm_openfiles(NULL, "/dev/null", NULL, 0, NULL);
	if (kd == NULL) {
		warn("kvm_openfiles '/dev/null'");
		return 0;
	}

	if (kvm_getswapinfo(kd, swap_info, size, 0) == -1) {
		warn("kvm_getswapinfo");
		kvm_close(kd);
		return 0;
	}

	kvm_close(kd);
	return 1;
}

void
swap_free(char *     out,
	  const char __unused * _a,
	  unsigned int __unused _i,
	  void __unused *_p)
{
	long int	used, total;
	struct kvm_swap swap_info[1];

	if (!getswapinfo(swap_info, 1)) ERRRET(out);

	total = swap_info[0].ksw_total;
	used  = swap_info[0].ksw_used;

	fmt_human(out, (total - used) * getpagesize());
}

void
swap_perc(char *     out,
	  const char __unused * _a,
	  unsigned int __unused _i,
	  void __unused *_p)
{
	long int	used, total;
	struct kvm_swap swap_info[1];

	if (!getswapinfo(swap_info, 1)) ERRRET(out);

	total = swap_info[0].ksw_total;
	used  = swap_info[0].ksw_used;

	bprintf(out, "%d", used * 100 / total);
}

void
swap_total(char *     out,
	   const char __unused * _a,
	   unsigned int __unused _i,
	   void __unused *_p)
{
	long int	total;
	struct kvm_swap swap_info[1];

	if (!getswapinfo(swap_info, 1)) ERRRET(out);

	total = swap_info[0].ksw_total;

	fmt_human(out, total * getpagesize());
}

void
swap_used(char *     out,
	  const char __unused * _a,
	  unsigned int __unused _i,
	  void __unused *_p)
{
	long int	used;
	struct kvm_swap swap_info[1];

	if (!getswapinfo(swap_info, 1)) ERRRET(out);

	used = swap_info[0].ksw_used;

	fmt_human(out, used * getpagesize());
}
