/* FUNCTIONS unit */
#ifndef _GATE_CONTROLLER_H
#define _GATE_CONTROLLER_H

#include <Arduino.h>


/* ----------- DEFINES ------------- */

/* ----------- VARIABLES sections ------------- */

/* ----------- FUNCTIONS section -------------- */
extern void initServo();
extern void setServoPosition(int value);
extern void doClickButton(int timeLength);
void cycleHandleServo();

/* -- no code below this line */
#endif
/* ---END OF FILE --- */
