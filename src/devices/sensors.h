/* FUNCTIONS unit */
#ifndef _SENSORS_H
#define _SENSORS_H

#include <Arduino.h>
// #include <OneWire.h>
// #include <DallasTemperature.h>

/* ----------- DEFINES ------------- */

/* ----------- VARIABLES sections ------------- */
extern float tempC;
extern char temperatureCString[];

extern int readVal0;
extern int readVal1;

extern int computedADC0;
extern int computedADC1;

extern boolean stableADC0;
extern boolean stableADC1;

extern float avg0;
extern float avg1;

/* ----------- FUNCTIONS section -------------- */
#if defined(ENABLE_MODULE_ONE_WIRE) && (ENABLE_MODULE_ONE_WIRE==true)	// OneWire ENABLED
extern void initTempSensor();
extern void updateTemp();
#endif

extern void initCurrentSensorsADC();
extern void updateCurrentSensorsADC();

/* -- no code below this line */
#endif
/* ---END OF FILE --- */
