/* FUNCTIONS unit */
#ifndef _HTTP_MODULE_H
#define _HTTP_MODULE_H

#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <ESP8266WebServer.h>
#include <ArduinoOTA.h>

#include "global.h"

extern void initWebServer();
extern void cyclicHandleWebRequests();

/* -- no code below this line */
#endif
/* ---END OF FILE --- */
