/* FUNCTIONS unit */
#ifndef _SETUP_H
#define _SETUP_H

#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <ESP8266WebServer.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

#include "global.h"
#include "system.h"

#include "ota.h"
#include "sensors.h"
#include "com.h"
#include "http_module.h"
#include "motor.h"
#include "gate_controlller.h"
#include "storage.h"

#define USE_ACTIVITY_LED true
#define TASK_CYCLIC_INTERVAL 7 // milliseconds (again ... prime number)
#define DEV_SELF_ACCESS_POINT true

extern void globalInit();

/* -- no code below this line */
#endif
/* ---END OF FILE --- */
