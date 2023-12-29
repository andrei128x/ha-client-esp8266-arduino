/* FUNCTIONS unit */
#ifndef _SENSORS_CURRENT_H
#define _SENSORS_CURRENT_H

#include <Arduino.h>
/* ----------- DEFINES ------------- */

/* ----------- VARIABLES sections ------------- */
#if defined(ENABLE_MODULE_SENSORS_CURRENT) && (ENABLE_MODULE_SENSORS_CURRENT == true)

extern int readVal0;
extern int readVal1;

extern int computedADC0;
extern int computedADC1;

extern boolean stableADC0;
extern boolean stableADC1;

extern float avg0;
extern float avg1;

/* ----------- FUNCTIONS section -------------- */
extern void initCurrentSensorsADC();
extern void updateCurrentSensorsADC();

#endif

/* -- no code below this line */
#endif
/* ---END OF FILE --- */
