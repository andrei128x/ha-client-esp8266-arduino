#include "../system/system.h"
#if defined(ENABLE_MODULE_SENSORS_CURRENT) && (ENABLE_MODULE_SENSORS_CURRENT == true)

/* FUNCTIONS unit */
#include "Arduino.h"
#include "../system/storage.h"
#include "./sensors_current.h"
#include "./gate_controlller.h"

#include <time.h>
#include "user_interface.h"
#include <Wire.h>
#include <Adafruit_ADS1015.h>
#include <Twi.h>

//#include <tmr.h>

/* ----------- DEFINES ------------- */
// #define ONBOARD_ADC A0

Adafruit_ADS1015 ads; /* Use this for the 12-bit version */ //TODO: Find what kind of chip is connected !!
// Adafruit_ADS1115 ads; /* Use this for the 12-bit version */ //TODO: does it work ?!
float fVoltsPerBitADC;

const int numvaluesFromADC = 53;			   // element count for the integrator filter of input values; prime number, to prevemnt 'moire' effect
const int numValuesForStabilityDetection = 71; // element count for considering the analog signal is stable; prime number, hehe

int valuesFromADC0[numvaluesFromADC]; // the valuesFromADC from the analog input
int valuesFromADC1[numvaluesFromADC];
int indexFilter0 = 0; // index for above tables of filter values
int indexFilter1 = 0;

int readVal0 = 0;
int readVal1 = 0;

float avg0 = readVal0;
float avg1 = readVal1;

int computedADC0 = 0;
int computedADC1 = 0;

boolean stableADC0 = false;
boolean stableADC1 = false;

/* ----------- FUNCTIONS -------------- */
void initCurrentSensorsADC()
{

	// initialize all the valuesFromADC to 0:
	for (int thisReading = 0; thisReading < numvaluesFromADC; thisReading++)
	{
		valuesFromADC0[thisReading] = 0;
		valuesFromADC1[thisReading] = 0;
	}

	ads.begin(D6, D5);

	// both of these two work, it seems
	// Wire.setClock(1000000);
	twi_setClock(1000000); // FIXME - this definitely works ...

	twi_setClock(3400000); // FIXME - 3.4 MHz !!! it seems to work !!!

	//fVoltsPerBitADC = ads.voltsPerBit() * 1000.0F; /* Sets the millivolts per bit */

	/* Use this to set data rate for the 12-bit version (optional)*/
	// ads.setSPS(ADS1015_DR_3300SPS); // for ADS1015 fastest samples per second is 3300 (default is 1600)
	/* Use this to set data rate for the 16-bit version (optional)*/
	ads.setSPS(ADS1115_DR_860SPS); // for ADS1115 fastest samples per second is 860 (default is 128)

	ads.setGain(GAIN_SIXTEEN);
	ads.startContinuous_Differential_0_1();
	ads.startContinuous_Differential_2_3();
	pinMode(D7, INPUT);
	digitalWrite(D7, HIGH);
};


static int stabilityCondition(int value)
{
	return value / 10 + 5; // ~3% stability condition, for current calibration values, approximately
}


/* circular buffer, where a new value is added at one end and an onld value is removed from the other end */
void calcMediaValoriCitite(int newValue, int *existingValue, int *valuesFilterTable, int *index)
{
	// subtract the last reading and the current one:
	*existingValue = *existingValue - valuesFilterTable[*index] + newValue;

	// read from the sensor:
	valuesFilterTable[*index] = newValue;

	// advance to the next position in the array:
	*index = *index + 1;

	// if we're at the end of the array...
	if (*index >= numvaluesFromADC)
	{
		// ...wrap around to the beginning:
		*index = 0;
	}
}


static void checkStabilityConditionForADCs(int adc0, int adc1)
{
	static int stabilityCounter = 0;

	static int lastValues0 = 0;
	static int lastValues1 = 0;

	boolean ret0 = true;
	boolean ret1 = true;

	if (abs(adc0 - lastValues0) >= stabilityCondition(adc0))
		ret0 = false;

	if (abs(adc1 - lastValues1) >= stabilityCondition(adc1))
		ret1 = false;

	lastValues0 = adc0;
	lastValues1 = adc1;

	if (ret0 && ret1)
		if (stabilityCounter < numValuesForStabilityDetection)
			stabilityCounter++;
		else
			;
	else
		stabilityCounter = 0;

	if (stabilityCounter >= numValuesForStabilityDetection)
	{

		stableADC0 = true;
		stableADC1 = true;
	}
	else
	{
		stableADC0 = false;
		stableADC1 = false;
	}
}


void updateCurrentSensorsADC()
{
	static char inputADCtoReadFrom = 0;
	int s16ADC0;
	int s16ADC1;

	unsigned long time_start = micros();
	unsigned long end;
	int pin_val;
	Serial.print((String)"" + __FILE__ +":" + __LINE__ + ":" + __FUNCTION__ + ":");

	if (inputADCtoReadFrom == 0)
	{
		static unsigned int timeoutCounter = 0;
		while ((pin_val = digitalRead(D7)) && (timeoutCounter++ < 500)) // FIXED stuck pin can cause infinite wait
			yield();
		if (timeoutCounter > 4000)
		{
			timeoutCounter = 0;
			initCurrentSensorsADC(); // reinit I2C communication with ADC sensor
			Serial.print("Reset ADCs\n");
		}

		readVal0 = ads.readADC_Differential_0_1();

		avg0 = (199.0 * avg0 + readVal0) / 200.0; // prime numbers
		s16ADC0 = readVal0 - avg0;
		calcMediaValoriCitite(s16ADC0 * s16ADC0, &computedADC0, valuesFromADC0, &indexFilter0);
	}
	else
	{
		static unsigned int timeoutCounter = 0;
		while ((pin_val = digitalRead(D7)) && (timeoutCounter++ < 500)) // FIXED stuck pin can cause infinite wait
			yield();
		if (timeoutCounter > 4000) // error condition
		{
			timeoutCounter = 0;
			initCurrentSensorsADC(); // reinit I2C communication with ADC sensor
			Serial.print("Reset ADCs\n");
		}

		readVal1 = ads.readADC_Differential_2_3();

		avg1 = (199.0 * avg1 + readVal1) / 200.0; // yeah, prime numbers... again
		s16ADC1 = readVal1 - avg1;
		calcMediaValoriCitite(s16ADC1 * s16ADC1, &computedADC1, valuesFromADC1, &indexFilter1);
	}

	end = micros();
	int elapsed = (int)end - time_start;

	inputADCtoReadFrom = 1 - inputADCtoReadFrom;

	checkStabilityConditionForADCs(computedADC0, computedADC1);

	static int cnt = 0;
	if (cnt == 0)
	{
		Serial.printf("ADC value: % d - %dus ", pin_val, elapsed);
		Serial.print(computedADC0 / (float)numvaluesFromADC);
		Serial.print(" ");
		Serial.print(computedADC1 / (float)numvaluesFromADC);
		Serial.print(" ");
		Serial.print((stableADC0 && stableADC1) * 5000);
		Serial.print(" ");
		Serial.print((float)readVal0 * fVoltsPerBitADC);
		Serial.print(" ");
		Serial.print((float)readVal1 * fVoltsPerBitADC);
		
#if defined(ENABLE_MODULE_GATE_CONTROLLER) && (ENABLE_MODULE_GATE_CONTROLLER == true)
		Serial.print(" ");
		Serial.print(gateState * 1000);
#endif

		// [fisier.c][line_no][function_name]

		// Serial.print(" ");
		// Serial.print(readVal0 * fVoltsPerBitADC);

		// Serial.print(" ");
		// Serial.print(readVal1 * fVoltsPerBitADC);

		Serial.println(" -50 50 "); // graphical scale
	}
	cnt = (cnt + 1) % 23;
};

/* ---END OF FILE --- */
#endif
