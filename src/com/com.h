/* FUNCTIONS unit */
#ifndef _COM_H
#define _COM_H

#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <ESP8266WebServer.h>

#include "../init/global.h"

/* ----------- DEFINES ------------- */
#define LOCAL_UDP_CONTROLLER_ADDRESS "webdev.local"
#define LOCAL_UDP_PORT 40000


/* ----------- VARIABLES sections ------------- */


/* ----------- FUNCTIONS section -------------- */
extern void sendWakeOnLan();

extern void initCOM();
extern void cyclicHandleRxUDP();
extern void sendAdcSensorDataUDP();

/* -- no code below this line */
#endif
/* ---END OF FILE --- */
