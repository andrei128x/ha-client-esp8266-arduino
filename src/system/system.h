/* SYSTEM unit */
#ifndef _SYSTEM_H
#define _SYSTEM_H

#include <Arduino.h>
#include "pins_arduino.h"
#include "../init/global.h"

/* ----------- SYSTEM MODULES ------------- */
#define ENABLE_MODULE_OTA true
#define ENABLE_MODULE_COM true
#define ENABLE_MODULE_HTTP true
#define ENABLE_MODULE_UDP_ADC_FORWARD true
#define ENABLE_MODULE_ONE_WIRE false
#define ENABLE_MODULE_GATE_CONTROLLER true
#define ENABLE_MODULE_SENSORS true
#define ENABLE_MODULE_MOTOR false
#define ENABLE_MODULE_EEPROM true

/* ----------- DEFINES ------------- */
#define SYS_TASK_LEN (10) // 10ms interval

#define ONBOARD_LED LED_BUILTIN

#define SYS_START_MEASUREMENT() time_t s1 = (time_t)(system_get_time());
#define SYS_STOP_MEASUREMENT() time_t s2 = (time_t)(system_get_time());
#define SYS_DBG_PRINT_MEASUREMENT()                               \
	{                                                             \
		Serial.print("CPU load: ");                               \
		Serial.print((s2 - s1) / (1000.0F / 100 * SYS_TASK_LEN)); \
		Serial.println("%");                                      \
	} // change microseconds to percentage

/* ----------- NOINIT VARIABLES sections ------------- */

extern unsigned long u32ResetType; /* contains last reset's type */

extern unsigned long u32WarmResetPattern __attribute__((section(".noinit"))); /* warm reset pattern, located in ResetSafe RAM */
extern unsigned long long u32TotalUptimeMS __attribute__((section(".noinit")));		  /* uptime before warm reset, located in ResetSafe RAM */
extern unsigned long u32ResetCounter __attribute__((section(".noinit")));

/* ----------- VARIABLES sections ------------- */

/* ----------- FUNCTIONS section -------------- */
extern void getSystemUptime(String *result);
extern void setActivityStateLED(activityState_type state);
extern void handleActivityLED();

extern void checkWarmFlag();
extern void u32AsciiToString(String *result, unsigned long value);

extern unsigned long ulSysTaskCnt;

/* -- no code below this line */
#endif
/* ---END OF FILE --- */
