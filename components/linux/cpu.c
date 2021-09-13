#include <err.h>
#include <math.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <sys/param.h> /* MIN, MAX */

#include "../cpu.h"
#include "../../lib/util.h"
#include "../../components_config.h"

/* clang-format off */
#define CPU_SUM(X)                                                            \
	(X[CPU_STATE_USER]                                                    \
	 + X[CPU_STATE_NICE]                                                  \
	 + X[CPU_STATE_SYSTEM]                                                \
	 + X[CPU_STATE_IDLE]                                                  \
	 + X[CPU_STATE_IOWAIT]                                                \
	 + X[CPU_STATE_IRQ]                                                   \
	 + X[CPU_STATE_SOFTIRQ]                                               \
	 + X[CPU_STATE_STEAL])

#define CPU_USED(X)                                                           \
	(X[CPU_STATE_USER]                                                    \
	 + X[CPU_STATE_NICE]                                                  \
	 + X[CPU_STATE_SYSTEM]                                                \
	 + X[CPU_STATE_IRQ]                                                   \
	 + X[CPU_STATE_SOFTIRQ]                                               \
	 + X[CPU_STATE_STEAL])
/* clang-format on */

void
cpu_freq(char *	    out,
	 const char __unused * _a,
	 unsigned int __unused _i,
	 void *		       static_ptr)
{
	uintmax_t freq;
	FILE **	  freq_fptr = static_ptr;

	if (sysfs_fptr(freq_fptr,
		       "/sys/devices/system/cpu",
		       "cpu0",
		       "cpufreq/scaling_cur_freq"))
		ERRRET(out);

	/* in kHz */
	if (fscanf(*freq_fptr, "%ju", &freq) != 1) ERRRET(out);

	fmt_human(out, freq * 1000);
}

void
cpu_perc(char *	    out,
	 const char __unused * _a,
	 unsigned int __unused _i,
	 void *		       static_ptr)
{
	struct cpu_data_t *data = static_ptr;

	__typeof__(*data->states) old_states[LEN(data->states)], sum;

	memcpy(old_states, data->states, sizeof(data->states));

	/* or not only sysfs :) */
	if (sysfs_fptr(&data->fptr, "/", "proc", "stat")) ERRRET(out);

	/* cpu user nice system idle iowait irq softirq */
	if (fscanf(data->fptr,
		   "cpu  %Lf %Lf %Lf %Lf %Lf %Lf %Lf %Lf",
		   &data->states[CPU_STATE_USER],
		   &data->states[CPU_STATE_NICE],
		   &data->states[CPU_STATE_SYSTEM],
		   &data->states[CPU_STATE_IDLE],
		   &data->states[CPU_STATE_IOWAIT],
		   &data->states[CPU_STATE_IRQ],
		   &data->states[CPU_STATE_SOFTIRQ],
		   &data->states[CPU_STATE_STEAL])
	    != LEN(data->states))
		ERRRET(out);

	if (!old_states[CPU_STATE_USER]) ERRRET(out);

	sum = CPU_SUM(old_states) - CPU_SUM(data->states);

	if (!sum || isnan(sum)) ERRRET(out);

	bprintf(
	    out,
	    "%hhu",
	    (uint8_t)(100.0 * (CPU_USED(old_states) - CPU_USED(data->states))
		      / sum));
}
