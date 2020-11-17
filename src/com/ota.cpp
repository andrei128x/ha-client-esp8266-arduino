#include "../system/system.h"
#if defined(ENABLE_MODULE_OTA) && (ENABLE_MODULE_OTA == true)

/* FUNCTIONS unit */
#include "../init/global.h"
#include "../system/system.h"
#include "../devices/sensors.h"
#include "../com/com.h"
#include "../devices/gate_controlller.h"

#include <WiFiUdp.h>

/* ---------------------------------- OTA ----------------------------------*/
/* --- hooks for OTA functions --- */
void onStartOTA()
{
	// switch off all the PWMs during upgrade
	//	for (int i = 0; i < N_DIMMERS; i++)
	//		analogWrite(dimmer_pin[i], 0);
	//analogWrite(led_pin,0);

	/* reset servo position */
	SPIFFS.end();
}

void onEndOTA()
{
	// do a fancy thing with our board led at end
	// for (int i = 0; i < 30; i++) {
	// 	//analogWrite(led_pin,(i*100) % 1001);
	// 	delay(50);
	// }
}

void startOTA(const char *host)
{
	Serial.println("Starting OTA...");

	ArduinoOTA.setHostname(host);
	//ArduinoOTA.setPassword((const char *)"1234");	// set password for OTA programming

	ArduinoOTA.onStart(onStartOTA);
	ArduinoOTA.onEnd(onEndOTA);
	ArduinoOTA.onError([](ota_error_t error)
	{	
		Serial.println("OTA error");
		ESP.restart();
	});

	ArduinoOTA.begin();
}

/* mandatory cyclic function for OTA reprogramming */
void cyclicHandleOTA()
{
	ArduinoOTA.handle();
}

/* ---END OF FILE --- */
#endif