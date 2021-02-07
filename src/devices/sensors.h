/* FUNCTIONS unit */
#ifndef _SENSORS_H
#define _SENSORS_H

#include <Arduino.h>
// #include <OneWire.h>
// #include <DallasTemperature.h>

/* ----------- DEFINES ------------- */

/* ----------- VARIABLES sections ------------- */

/* ----------- FUNCTIONS section -------------- */
extern char temperatureCString[];

#if defined(ENABLE_MODULE_SENSORS_ONE_WIRE) && (ENABLE_MODULE_SENSORS_ONE_WIRE == true) // OneWire ENABLED
extern float tempC;
extern void initTempSensor();
extern void updateTemp();
#endif

#if defined(ENABLE_MODULE_SENSORS_ADS1x15) && (ENABLE_MODULE_SENSORS_ADS1x15 == true)
extern int readVal0;
extern int readVal1;

extern int computedADC0;
extern int computedADC1;

extern boolean stableADC0;
extern boolean stableADC1;

extern float avg0;
extern float avg1;

extern void initCurrentSensorsADC();
extern void updateCurrentSensorsADC();
#endif

/* -- no code below this line */
#endif
/* ---END OF FILE --- */
