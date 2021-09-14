#ifndef BATTERY_H
#define BATTERY_H

#include "../os.h"
#include "../components_config.h"

#ifndef BATTERY_FULL
#	define BATTERY_FULL "" /* "f" */
#endif

#ifndef BATTERY_UNKNOWN
#	define BATTERY_UNKNOWN "" /* "?" */
#endif

#ifndef BATTERY_CHARGING
#	define BATTERY_CHARGING "+" /* "+" */
#endif

#ifndef BATTERY_DISCHARGING
#	define BATTERY_DISCHARGING "-" /* "-" */
#endif

#ifndef BATTERY_REMAINING_NOT_DISCHARGING
#	define BATTERY_REMAINING_NOT_DISCHARGING "?"
#endif

#if LINUX
struct remaining {
	int status, charge, current;
};
#	define BATTERY_REMAINING_STATIC_SIZE sizeof(struct remaining)
#else
#	define BATTERY_REMAINING_STATIC_SIZE 0
#endif

#endif /* BATTERY_H */
