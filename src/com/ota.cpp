#include "../system/system.h"
#if defined(ENABLE_MODULE_OTA) && (ENABLE_MODULE_OTA == true)

/* FUNCTIONS unit */
#include "../init/global.h"
#include "../system/system.h"
#include "../devices/sensors.h"
#include "../com/ota.h"
#include "../devices/gate_controlller.h"

// #include <WiFiUdp.h>

/* ---------------------------------- OTA ----------------------------------*/
/* --- hooks for OTA functions --- */
void onStartOTA()
{
	/* reset servo position */

	// SPIFFS.end();	//disabled for ESP32

	//TODO investigate OTA procedure below, from Basic OTA example
	// String type;
    // if (ArduinoOTA.getCommand() == U_FLASH) {
    //   type = "sketch";
    // } else { // U_FS
    //   type = "filesystem";
    // }

    // // NOTE: if updating FS this would be the place to unmount FS using FS.end()
    // Serial.println("Start updating " + type);
}

void onEndOTA()
{

}

void startOTA()
{
	Serial.println("Starting OTA...");

	// ArduinoOTA.setHostname(host);	// FIXME check whether it makes sense to have a pre-defined name for the board, in the context of self advertising
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