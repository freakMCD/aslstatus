#include <err.h>
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
	int *	  fd = static_ptr;
	char	  buf[JU_STR_SIZE];

	SYSFS_FD_OR_SEEK({ ERRRET(out); },
			 *fd,
			 "/sys/devices/system/cpu",
			 "cpu0",
			 "cpufreq/scaling_cur_freq");

	EREAD({ ERRRET(out); }, _unused, *fd, WITH_LEN(buf));

	/* in kHz */
	SCANF({ ERRRET(out); }, 1, sscanf, buf, "%ju", &freq);

	fmt_human(out, freq * 1000);
}

void
cpu_perc(char *	    out,
	 const char __unused * _a,
	 unsigned int __unused _i,
	 void *		       static_ptr)
{
	struct cpu_data_t *data = static_ptr;

	char buf[STR_SIZE("cpu ") + JU_STR_SIZE * LEN(data->states)];

	__typeof__(*data->states) old_states[LEN(data->states)], sum;
	__typeof__(sum)		  tmp_sum, old_sum;

	memcpy(old_states, data->states, sizeof(data->states));

	SYSFS_FD_OR_SEEK({ ERRRET(out); }, data->fd, "/", "proc", "stat");

	EREAD({ ERRRET(out); }, _unused, data->fd, WITH_LEN(buf));

	/* cpu user nice system idle iowait irq softirq */
	SCANF({ ERRRET(out); },
	      LEN(data->states),
	      sscanf,
	      buf,
	      "cpu  %ju %ju %ju %ju %ju %ju %ju %ju",
	      &data->states[CPU_STATE_USER],
	      &data->states[CPU_STATE_NICE],
	      &data->states[CPU_STATE_SYSTEM],
	      &data->states[CPU_STATE_IDLE],
	      &data->states[CPU_STATE_IOWAIT],
	      &data->states[CPU_STATE_IRQ],
	      &data->states[CPU_STATE_SOFTIRQ],
	      &data->states[CPU_STATE_STEAL]);

	if (!old_states[CPU_STATE_USER]) ERRRET(out);

#define ABS_DEC(A, B) (MAX((A), (B)) - MIN((A), (B)))

	old_sum = CPU_SUM(old_states);
	tmp_sum = CPU_SUM(data->states);
	sum	= ABS_DEC(old_sum, tmp_sum);

	if (!sum) ERRRET(out);

	old_sum = CPU_USED(old_states);
	tmp_sum = CPU_USED(data->states);

	bprintf(out, "%hhu", (uint8_t)(100 * ABS_DEC(old_sum, tmp_sum) / sum));
}
