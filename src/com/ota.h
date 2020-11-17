/* FUNCTIONS unit */
#ifndef _OTA_H
	#define _OTA_H

#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <ESP8266WebServer.h>
#include <ArduinoOTA.h>

#include "../init/global.h"


extern void cyclicHandleOTA();
extern void startOTA(const char *host);

/* -- no code below this line */
#endif
/* ---END OF FILE --- */
