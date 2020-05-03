/* FUNCTIONS unit */
#ifndef _GATE_CONTROLLER_H
#define _GATE_CONTROLLER_H

#include <Arduino.h>


/* ----------- DEFINES ------------- */
typedef enum
{
    gate_init = 0,
    gate_idle = 1,
    gate_debounce = 2,
    gate_opening = 3,
    gate_opened = 4,
    gate_closing = 5,
    gate_closed = 6
} servo_state_type;

/* ----------- VARIABLES sections ------------- */
extern servo_state_type gateState;
extern int debounceTimer;

/* ----------- FUNCTIONS section -------------- */
extern void initGateStates();

extern void setServoPosition(int value);
extern void doClickRelay(int timeLength);
void cycleHandleServo();

/* -- no code below this line */
#endif
/* ---END OF FILE --- */
