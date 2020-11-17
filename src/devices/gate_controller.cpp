#include "../system/system.h"
#if defined(ENABLE_MODULE_GATE_CONTROLLER) && (ENABLE_MODULE_GATE_CONTROLLER == true)

/* FUNCTIONS unit */
#include "../init/setup.h"
#include "./gate_controlller.h"
#include "./sensors.h"

/* ----------- DEFINES ------------- */

#define RELAY_PIN D1

#define MINIMUM_DIFFERENCE 50000
#define SENSORS_HIGH_THRESHOLD 100000

servo_state_type gateState = gate_init;

/*------------ VARIABLES -------------- */
int debounceTimer = 0;

/* ----------- FUNCTIONS -------------- */
void initGateStates()
{
    gateState = gate_idle;
}

boolean isMovementDetected()
{
    return ((computedADC0 > SENSORS_HIGH_THRESHOLD) || (computedADC1 > SENSORS_HIGH_THRESHOLD));
}

void updateStates(int timeLength)
{
    static int countDown = 0;

    if (timeLength > 0)
        countDown = timeLength;

    if (countDown > 0)
    {
        countDown--;
        Serial.println(countDown);

        if (countDown == 0)
        {
            digitalWrite(RELAY_PIN, LOW); // OFF
        }
    }
}

/* start the state machine to perform a "click" action, with a pre-defined time limit */
void doClickRelay(int timeLength)
{
    static unsigned char relayState = 0;

    relayState = 1 - relayState;

    /* move the servo to  */
    pinMode(RELAY_PIN, OUTPUT);
    digitalWrite(RELAY_PIN, HIGH); // ON

    updateStates(5);

    /* start timeout timer */
    debounceTimer = timeLength;
}

/* mandatory periodic function for servo functions */
void cycleHandleServo()
{
    switch (gateState)
    {

    case gate_idle:
    case gate_opened:
    case gate_closed:
        if (isMovementDetected())
        {
            gateState = gate_debounce;
            // debounceTimer = DEBOUNCE_DELAY / TASK_CYCLIC_INTERVAL;
        };
        break;

    case gate_debounce:

        // if (debounceTimer == 0)
        if (stableADC0 && stableADC1)
        {
            if (isMovementDetected())
            {
                if (computedADC0 * 0.8 > computedADC1)
                    gateState = gate_opening;
                else if (computedADC1 * 0.8 > computedADC0)
                    gateState = gate_closing;
            }
            else
            {
                gateState = gate_idle;
            }
        }
        break;

    case gate_opening:
        if (!isMovementDetected())
            gateState = gate_opened;
        break;

    case gate_closing:
        if (!isMovementDetected())
            gateState = gate_closed;
        break;

    default:
        break;
    }

    if (debounceTimer > 0)
        debounceTimer--;

    updateStates(0);
}

/* ---END OF FILE --- */
#endif