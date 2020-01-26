#include "system.h"
#if defined(ENABLE_MODULE_GATE_CONTROLLER) && (ENABLE_MODULE_GATE_CONTROLLER == true)

/* FUNCTIONS unit */
#include "gate_controlller.h"
#include <Servo.h>

/* ----------- DEFINES ------------- */

#define ONBOARD_SERVO D2

#define SERVO_CLICK_LENGTH 1200 // 1000ms for the servo's click operation

typedef enum
{
    servo_init,
    servo_up,
    servo_down
} servo_state_type;

servo_state_type servoState = servo_init;

/*------------ VARIABLES -------------- */

Servo myservo; // create servo object to control a servo
int timer = 0;

/* ----------- FUNCTIONS -------------- */
void initServo()
{
    myservo.attach(D2);
    myservo.write(0);
    servoState = servo_up;
}

void stopServo()
{
    myservo.detach();
};

/* immediately set servo position */
void setServoPosition(int value)
{

    if (value < 0)
        value -= 0;
    if (value > 180)
        value = 180;

    myservo.write(value);
}

/* start the state machine to perform a "click" action, with a pre-defined time limit */
void doClickButton(int timeLength)
{
    /* move the servo to  */
    //setServoPosition(55);
    setServoPosition(50);
    servoState = servo_down;

    /* start timeout timer */
    timer = timeLength;
}

/* mandatory periodic function for servo functions */
void cycleHandleServo()
{
    if (timer > 0)
    {
        timer--;
        if (timer == 0)
        {
            /* timer reached stop state, move servo to resting state */
            setServoPosition(0);
            //stopServo();
        }
    }

    // Serial.print("servo timer: ");
    // Serial.println(timer);
}

/* ---END OF FILE --- */
#endif