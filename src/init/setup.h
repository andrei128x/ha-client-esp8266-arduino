/* FUNCTIONS unit */
#ifndef _SETUP_H
#define _SETUP_H

#include <WiFi.h>
// #include <WiFiUdp.h>
// #include <WebServer.h>
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

#if defined(ENABLE_MODULE_SENSORS_ONE_WIRE) && (ENABLE_MODULE_SENSORS_ONE_WIRE == true)
#define TASK_CYCLIC_INTERVAL 500 // milliseconds
#endif

#if defined(ENABLE_MODULE_SENSORS_ADS1x15) && (ENABLE_MODULE_SENSORS_ADS1x15 == true)
#define TASK_CYCLIC_INTERVAL 1 // milliseconds (again ... prime number is desirable for the ADC; anti-aliasing)
#endif

#define DEV_SELF_ACCESS_POINT true

extern void globalInitNoWiFi();
extern void globalInitWiFiAvailable();

/* -- no code below this line */
#endif
/* ---END OF FILE --- */
