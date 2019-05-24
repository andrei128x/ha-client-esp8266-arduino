/* FUNCTIONS unit */
#ifndef _COM_H
	#define _COM_H

#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <ESP8266WebServer.h>
#include <ArduinoOTA.h>

#include "global.h"

extern void sendWakeOnLan();

extern void cyclicHandleOTA();
extern void startOTA(const char *host);


extern void initWebServer();
extern void cyclicHandleWebRequests();
extern void serverHandleJsonRequest();


/* -- no code below this line */
#endif
/* ---END OF FILE --- */
