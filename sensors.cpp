#include "system.h"
#if defined(ENABLE_MODULE_SENSORS) && (ENABLE_MODULE_SENSORS == true)

/* FUNCTIONS unit */
#include "sensors.h"
#include "Arduino.h"

#include <time.h>
#include "user_interface.h"
#include <Wire.h>
#include <Adafruit_ADS1015.h>
#include <Twi.h>

//#include <tmr.h>

/* ----------- DEFINES ------------- */
#define myPeriodic 15   //in sec | Thingspeak pub is 15sec
#define ONE_WIRE_BUS D4 // DS18B20 on arduino pin2 corresponds to D4 on physical board

#define ONBOARD_ADC A0

Adafruit_ADS1015 ads; /* Use this for the 12-bit version */
float multiplier;

#if defined(ENABLE_MODULE_ONE_WIRE) && (ENABLE_MODULE_ONE_WIRE == true) // OneWire ENABLED
/*------------ VARIABLES -------------- */
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature DS18B20(&oneWire);
#endif

float tempC = -150;

char temperatureCString[6];
//char temperatureFString[6];

const int numvaluesFromADC = 71; // element count for the integrator filter of input values; prime number, to prevemnt 'moire' effect

int valuesFromADC0[numvaluesFromADC]; // the valuesFromADC from the analog input
int valuesFromADC1[numvaluesFromADC];
int indexFilter0 = 0; // index for above tables of filter values
int indexFilter1 = 0;

int readVal0 = 449;
int readVal1 = 449;

float avg0 = readVal0;
float avg1 = readVal1;

int computedADC0 = 0;
int computedADC1 = 0;

/* ----------- FUNCTIONS -------------- */

#if defined(ENABLE_MODULE_ONE_WIRE) && (ENABLE_MODULE_ONE_WIRE == true) // OneWire ENABLED
void initTempSensor()
{
	DS18B20.begin();
	DS18B20.setWaitForConversion(false);
	DS18B20.requestTemperatures();
}

void updateTemp()
{
	//DS18B20.requestTemperatures();
	//return DS18B20.getTempCByIndex(0);

	// preform the OneWire reading
	if (DS18B20.isConversionComplete())
	{
		tempC = DS18B20.getTempCByIndex(0); // transfer takes ~27 milliseconds
		DS18B20.requestTemperatures();		// reading takes ~600 milliseconds
											//Serial.println("complete");
	}
	else
	{
		//		Serial.println("NOT complete");
	}

	// convert from float to char[]
	dtostrf(tempC, 2, 2, temperatureCString);
}
#endif // OneWire temperature sensor

void initCurrentSensorsADC()
{

	// initialize all the valuesFromADC to 0:
	for (int thisReading = 0; thisReading < numvaluesFromADC; thisReading++)
	{
		valuesFromADC0[thisReading] = 0;
		valuesFromADC1[thisReading] = 0;
	}


	ads.begin();
	// Wire.setClock(1000000);
	twi_setClock(1000000);

	//multiplier = ads.voltsPerBit() * 1000.0F; /* Sets the millivolts per bit */

	/* Use this to set data rate for the 12-bit version (optional)*/
	ads.setSPS(ADS1015_DR_3300SPS); // for ADS1015 fastest samples per second is 3300 (default is 1600)

	ads.startContinuous_Differential_0_1();
	ads.startContinuous_Differential_2_3();
	/* Use this to set data rate for the 16-bit version (optional)*/
	// ads.setSPS(ADS1115_DR_860SPS);      // for ADS1115 fastest samples per second is 860 (default is 128)
};

void calcMediaValoriCitite(int newValue, int *existingValue, int *valuesFilterTable, int *index)
{
	// subtract the last reading:
	*existingValue = *existingValue - valuesFilterTable[*index];

	// read from the sensor:
	valuesFilterTable[*index] = newValue;

	// add the reading to the total:
	*existingValue = *existingValue + valuesFilterTable[*index];

	// advance to the next position in the array:
	*index = *index + 1;

	// if we're at the end of the array...
	if (*index >= numvaluesFromADC)
	{
		// ...wrap around to the beginning:
		*index = 0;
	}
}

void updateCurrentSensorsADC()
{
	static char inputADCtoReadFrom = 0;
	int s16ADC0;
	int s16ADC1;

	if (inputADCtoReadFrom == 0)
	{
		readVal0 = ads.readADC_Differential_0_1();
		avg0 = (30.0 * avg0 + readVal0) / 31.0; // prime numbers
		s16ADC0 = readVal0 - avg0;
		calcMediaValoriCitite(s16ADC0 * s16ADC0, &computedADC0, valuesFromADC0, &indexFilter0);

	}
	else
	{
		readVal1 = ads.readADC_Differential_2_3();
		avg1 = (30.0 * avg1 + readVal1) / 31.0; // yeah, prime numbers... again
		s16ADC1 = readVal1 - avg1;
		calcMediaValoriCitite(s16ADC1 * s16ADC1, &computedADC1, valuesFromADC1, &indexFilter1);

	}

	inputADCtoReadFrom = 1 - inputADCtoReadFrom;

	// sum-filter of values from the input

	static int cnt = 0;
	if (cnt == 0)
	{
		Serial.print("ADC value: ");
		Serial.print(450 + computedADC0 / (float)numvaluesFromADC);
		Serial.print(" ");
		Serial.print(450 + computedADC1 / (float)numvaluesFromADC);
		Serial.print(" ");
		Serial.print(readVal0);
		Serial.print(" ");
		Serial.print(readVal1);

		Serial.print("Differential 0-1: ");
		Serial.print(readVal0 * multiplier);
		
		Serial.print("Differential 2-3: ");
		Serial.print(readVal1 * multiplier);

		Serial.println(" 430 520 ");
	}

	cnt = (cnt + 1) % 25;
};

/* ---END OF FILE --- */
#endif