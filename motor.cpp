#include "system.h"
#if defined(ENABLE_MODULE_MOTOR) && (ENABLE_MODULE_MOTOR == true)

/* FUNCTIONS unit */
#include "sensors.h"
#include <Servo.h>

/* ----------- DEFINES ------------- */
#define ESC_PIN D0
#define ESC_MIN_VAL 800

typedef enum {
	esc_init, esc_wait_zero, esc_wait_arm, esc_ready
} esc_state_type;

/*------------ VARIABLES -------------- */
Servo firstESC;
esc_state_type state = esc_init;

/* ----------- FUNCTIONS -------------- */
void initMotorControl() {
	pinMode(ESC_PIN, OUTPUT);
	firstESC.attach(ESC_PIN);
	firstESC.writeMicroseconds((int)0);
	state = esc_wait_zero;
}

void updateMotorSpeed() {
	static int contor = 0;

	if (esc_wait_zero == state) {
		contor++;
		if (contor == 50) {
			firstESC.writeMicroseconds((int)ESC_MIN_VAL);
			state = esc_wait_arm;
			contor = 0;
		}
	}

	if (esc_wait_arm == state) {
		contor++;
		if (contor == 30) {
			state = esc_ready;
		}
	}

	if (esc_ready == state) {
		int val = ESC_MIN_VAL + (1000-ESC_MIN_VAL)*(tempC-24)/3;
		firstESC.writeMicroseconds((int) val);

		Serial.print("Computed PWM value: "); 
		Serial.println(val);
	}

	Serial.print("esc state: ");
	Serial.println(state);
	Serial.println();
}

/* ---END OF FILE --- */
#endif