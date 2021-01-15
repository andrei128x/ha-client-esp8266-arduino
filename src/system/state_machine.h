/* SYSTEM unit */
#ifndef _STATE_MACHINE_H
#define _STATE_MACHINE_H

#include <Arduino.h>
#include "pins_arduino.h"
#include "../init/global.h"

/* ----------- DEFINES ------------- */
typedef enum
{
    SM_STATE_NO_INIT = 0,
    SM_STATE_STARTUP_NO_WIFI,
    SM_STATE_STARTUP_WIFI,
    SM_STATE_RUNNING_WIFI,
    SM_STATE_RUNNING_WIFI_DISCONNECTED
} machine_states_type;

typedef struct
{
    char name[48];
    unsigned char sm_id;
    unsigned char init;
} current_state_t;

/* ----------- NOINIT VARIABLES sections ------------- */

/* ----------- VARIABLES sections ------------- */
extern int some_variable_here;

/* ----------- FUNCTIONS section -------------- */
// extern unsigned void some_function_here();
extern void vDoInitSM();

extern void enter_SM_STATE_NO_INIT();
extern void enter_SM_STATE_STARTUP_NO_WIFI();
extern void update_SM_STATE_STARTUP_NO_WIFI();
extern void enter_SM_STATE_STARTUP_WIFI();
extern void enter_SM_STATE_RUNNING_WIFI();
extern void update_SM_STATE_RUNNING_WIFI();
extern void update_SM_STATE_RUNNING_WIFI_DISCONNECTED();

extern void doRequestTransition(machine_states_type requested_state);
extern boolean isNetworkAvailable();
extern void doExecute();

/* -- no code below this line */
#endif
/* ---END OF FILE --- */
