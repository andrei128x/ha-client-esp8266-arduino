/* FUNCTIONS unit */
#include "sensors.h"
#include "Arduino.h"

#include <time.h>
extern "C"{
  #include "user_interface.h"
}

//#include <tmr.h>

/* ----------- DEFINES ------------- */
#define myPeriodic 15 //in sec | Thingspeak pub is 15sec
#define ONE_WIRE_BUS D4 // DS18B20 on arduino pin2 corresponds to D4 on physical board

/*------------ VARIABLES -------------- */
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature DS18B20(&oneWire);

float tempC = -150;

char temperatureCString[6];
//char temperatureFString[6];

/* ----------- FUNCTIONS -------------- */
void initTempSensor() {
	DS18B20.begin();
	DS18B20.setWaitForConversion(false);
	DS18B20.requestTemperatures();
}

void updateTemp() {
	//DS18B20.requestTemperatures();
	//return DS18B20.getTempCByIndex(0);



	// save time before reading
	time_t s1 = (time_t)(system_get_time());

	// preform the OneWire reading


	if(DS18B20.isConversionComplete()){
		tempC = DS18B20.getTempCByIndex(0);	// transfer takes ~27 milliseconds
		DS18B20.requestTemperatures();		// reading takes ~600 milliseconds
		//Serial.println("complete");
	}else{
//		Serial.println("NOT complete");
	}

	//save time after reading
	time_t s2 = (time_t)(system_get_time());


	// output instrumentation data
	//Serial.print("transfer time (us): ");
	//Serial.println((s2-s1));

	// convert from float to char[]
	dtostrf(tempC, 2, 2, temperatureCString);
}
