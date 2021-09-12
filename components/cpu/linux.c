#include <err.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "cpu.h"
#include "../../util.h"

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

	__typeof__(*data->states) old_states[7], sum;

	memcpy(old_states, data->states, sizeof(old_states));

	/* or not only sysfs :) */
	if (sysfs_fptr(&data->fptr, "/", "proc", "stat")) ERRRET(out);

	/* cpu user nice system idle iowait irq softirq */
	if (fscanf(data->fptr,
		   "%*s %ju %ju %ju %ju %ju %ju %ju",
		   &data->states[0],
		   &data->states[1],
		   &data->states[2],
		   &data->states[3],
		   &data->states[4],
		   &data->states[5],
		   &data->states[6])
	    != LEN(data->states))
		ERRRET(out);

	if (!old_states[0]) ERRRET(out);

#define SUM7(X)	      (X[0] + X[1] + X[2] + X[3] + X[4] + X[5] + X[6])
#define ABS_DEC(A, B) (MAX((A), (B)) - MIN((A), (B)))
	/*
	 * preprocessor will generate scary code, but it will be very good
	 * optimized by compiler with any number of optimization, except -O0
	 */

	if (!(sum = ABS_DEC(SUM7(old_states), SUM7(data->states))))
		ERRRET(out);

#define SUM01256(X) (X[0] + X[1] + X[2] + X[5] + X[6])
	bprintf(
	    out,
	    "%hhu",
	    (uint8_t)(100
		      * ABS_DEC(SUM01256(old_states), SUM01256(data->states))
		      / sum));
}
