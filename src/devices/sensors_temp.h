/* FUNCTIONS unit */
#ifndef _SENSORS_H
#define _SENSORS_H

#include <Arduino.h>

#if defined(ENABLE_MODULE_SENSORS_ONE_WIRE_TEMP) && (ENABLE_MODULE_SENSORS_ONE_WIRE_TEMP==true)	// OneWire ENABLED

#include <OneWire.h>
#include <DallasTemperature.h>

/* ----------- DEFINES ------------- */

/* ----------- VARIABLES sections ------------- */
extern float tempC;
extern char temperatureCString[];

/* ----------- FUNCTIONS section -------------- */
extern void initTempSensor();
extern void updateTemp();
#endif

/* -- no code below this line */
#endif
/* ---END OF FILE --- */
