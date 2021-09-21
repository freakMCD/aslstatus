#include <err.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <sys/param.h> /* MIN, MAX */

#include "../cpu.h"
#include "../../lib/util.h"
#include "../../aslstatus.h"
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

static void cpu_perc_cleanup(void *ptr);

void
cpu_freq(char *	    out,
	 const char __unused *_a,
	 uint32_t __unused    _i,
	 static_data_t *      static_data)
{
	uintmax_t freq;
	int *	  fd = static_data->data;
	char	  buf[JU_STR_SIZE];

	if (!static_data->cleanup) static_data->cleanup = fd_cleanup;

	if (!sysfs_fd_or_rewind(fd,
				"/sys/devices/system/cpu",
				"cpu0",
				"cpufreq/scaling_cur_freq"))
		ERRRET(out);

	if (!eread(*fd, WITH_LEN(buf))) ERRRET(out);

	/* in kHz */
	if (!esscanf(1, buf, "%ju", &freq)) ERRRET(out);

	fmt_human(out, freq * 1000);
}

void
cpu_perc(char *	    out,
	 const char __unused *_a,
	 uint32_t __unused    _i,
	 static_data_t *      static_data)
{
	struct cpu_data_t *data = static_data->data;

	char buf[STR_SIZE("cpu ") + JU_STR_SIZE * LEN(data->states)];

	__typeof__(*data->states) old_states[LEN(data->states)], sum;
	__typeof__(sum)		  tmp_sum, old_sum;

	if (!static_data->cleanup) static_data->cleanup = cpu_perc_cleanup;

	memcpy(old_states, data->states, sizeof(data->states));

	if (!sysfs_fd_or_rewind(&data->fd, "/", "proc", "stat")) ERRRET(out);

	if (!eread(data->fd, WITH_LEN(buf))) ERRRET(out);

	/* cpu user nice system idle iowait irq softirq */
	if (!esscanf(LEN(data->states),
		     buf,
		     "cpu  %ju %ju %ju %ju %ju %ju %ju %ju",
		     &data->states[CPU_STATE_USER],
		     &data->states[CPU_STATE_NICE],
		     &data->states[CPU_STATE_SYSTEM],
		     &data->states[CPU_STATE_IDLE],
		     &data->states[CPU_STATE_IOWAIT],
		     &data->states[CPU_STATE_IRQ],
		     &data->states[CPU_STATE_SOFTIRQ],
		     &data->states[CPU_STATE_STEAL]))
		ERRRET(out);

	if (!old_states[CPU_STATE_USER]) ERRRET(out);

#define ABS_DEC(A, B) (MAX((A), (B)) - MIN((A), (B)))

	old_sum = CPU_SUM(old_states);
	tmp_sum = CPU_SUM(data->states);
	sum	= ABS_DEC(old_sum, tmp_sum);

	if (!sum) ERRRET(out);

	old_sum = CPU_USED(old_states);
	tmp_sum = CPU_USED(data->states);

	bprintf(out,
		"%" PRIperc,
		(percent_t)(100 * ABS_DEC(old_sum, tmp_sum) / sum));
}

static inline void
cpu_perc_cleanup(void *ptr)
{
	eclose(((struct cpu_data_t *)ptr)->fd);
}
