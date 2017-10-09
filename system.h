/* SYSTEM unit */
#ifndef _SYSTEM_H
	#define _SYSTEM_H

#include <Arduino.h>
#include "global.h"

/* ----------- DEFINES ------------- */

/* ----------- NOINIT VARIABLES sections ------------- */

extern unsigned long u32ResetType;	/* contains last reset's type */

extern unsigned long u32WarmResetPattern __attribute__ ((section (".noinit")));	/* warm reset pattern, located in ResetSafe RAM */
extern unsigned long long msUptime __attribute__ ((section (".noinit")));		/* uptime before warm reset, located in ResetSafe RAM */

/* ----------- VARIABLES sections ------------- */


/* ----------- FUNCTIONS section -------------- */
extern void getSystemUptime(String *result);
extern void setActivityStateLED(activityState_type state);
extern void handleActivityLED();

extern void checkWarmFlag();
extern void u32AsciiToString(String *result, unsigned long value);


/* -- no code below this line */
#endif
/* ---END OF FILE --- */
