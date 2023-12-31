#include "../system/system.h"
#if defined(ENABLE_MODULE_SENSORS_ONE_WIRE_TEMP) && (ENABLE_MODULE_SENSORS_ONE_WIRE_TEMP == true) // OneWire ENABLED

/* FUNCTIONS unit */
#include "Arduino.h"
#include "../system/storage.h"
#include "./sensors_temp.h"
#include "./gate_controlller.h"

#include <time.h>
#include "user_interface.h"
// #include <Wire.h>
#include <Twi.h>

// #include <tmr.h>

/* ----------- DEFINES ------------- */
#define ONE_WIRE_BUS D4 // DS18B20 on arduino pin2 corresponds to D4 on physical board
#define TOTAL_DEVICES 8
#define ONE_WIRE_ADDR_SIZE 8

/*------------ VARIABLES -------------- */
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors_DS18B20(&oneWire);

float tempC = -150;
char temperatureCString[128] = "[]";

byte sensorsCount = 0; // should enumerate only DS18B20 (enumerate all, then filter only the temperature sensors), but... yeah

unsigned char deviceList[TOTAL_DEVICES][ONE_WIRE_ADDR_SIZE] = {0};

/* ----------- FUNCTIONS -------------- */

void initTempSensor()
{
	sensors_DS18B20.begin();

	sensorsCount = sensors_DS18B20.getDS18Count();
	if (sensorsCount)
	{
		Serial.printf(" %d devices:\n", sensorsCount);

		for (byte idx = 0; idx < sensorsCount; idx++)
		{
			sensors_DS18B20.getAddress(deviceList[idx], idx);

			for (byte i = 0; i < ONE_WIRE_ADDR_SIZE; i++)
				Serial.printf("%02X ", deviceList[idx][i]);
			Serial.println();
		}
	}
	else
		Serial.println(" no oneWire devices found");

	// sensors_DS18B20.setWaitForConversion(false);
	sensors_DS18B20.requestTemperatures();
}

void updateTemps()
{
	static byte idx = 0;
	// sensors_DS18B20.requestTemperatures();
	// return sensors_DS18B20.getTempCByIndex(0);

	// preform the OneWire reading
	// if (sensors_DS18B20.isConversionComplete())
	// {
	tempC = sensors_DS18B20.getTempC(deviceList[idx]); // transfer takes ~27 milliseconds
	char temp[8] = {0};
	static char tempData[64] = "[ ";

	dtostrf(tempC, 2, 2, temp);

	if (idx > 0)
		strcat(tempData, ", ");

	strcat(tempData, temp);

	idx++;
	if (idx >= sensorsCount)
	{
		idx = 0;
		sensors_DS18B20.requestTemperatures(); // reading takes ~600 milliseconds

		temperatureCString[0] = 0;

		strcat(tempData, " ]");
		strcat(temperatureCString, tempData);
		
		bzero(tempData, std::size(tempData));
		tempData[0] = 0;
		strcat(tempData, "[ ");
	}
	// Serial.println("complete");
	// }
	// else
	// {
	// 	//		Serial.println("NOT complete");
	// }

	// convert from float to char[]
	// dtostrf(tempC, 2, 2, temperatureCString);
}

/* ---END OF FILE --- */
#endif
