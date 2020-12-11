/* FUNCTIONS unit */
#ifndef _SETUP_H
#define _SETUP_H

#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <ESP8266WebServer.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

#include "./global.h"
#include "../system/system.h"

#include "../com/com.h"
#include "../com/ota.h"
#include "../com/http_module.h"
#include "../system/storage.h"
#include "../devices\sensors.h"
#include "../devices\motor.h"
#include "../devices\gate_controlller.h"

#define USE_ACTIVITY_LED true
#define TASK_CYCLIC_INTERVAL 1 // milliseconds (again ... prime number)
#define DEV_SELF_ACCESS_POINT true

extern void globalInitNoWiFi();
extern void globalInitWiFiAvailable();

/* -- no code below this line */
#endif
/* ---END OF FILE --- */
