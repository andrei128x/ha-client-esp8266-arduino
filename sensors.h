/* FUNCTIONS unit */
#ifndef _SENSORS_H
#define _SENSORS_H

#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>

/* ----------- DEFINES ------------- */

/* ----------- VARIABLES sections ------------- */
extern float tempC;
extern char temperatureCString[];

/* ----------- FUNCTIONS section -------------- */
extern void initTempSensor();
extern void updateTemp();


/* -- no code below this line */
#endif
/* ---END OF FILE --- */
