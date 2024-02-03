/* FUNCTIONS unit */
#ifndef _COM_H
#define _COM_H

#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <ESP8266WebServer.h>

#include "../init/global.h"

/* ----------- DEFINES ------------- */
// #define SERVER_UDP_CONTROLLER_ADDRESS "webdev.local"
#define SERVER_UDP_CONTROLLER_ADDRESS "192.168.1.100"
#define SERVER_UDP_PORT 40000

#define LOCAL_UDP_PORT 2001

/* ----------- VARIABLES sections ------------- */

/* ----------- FUNCTIONS section -------------- */

class Com
{
private:
    boolean initialized = false;

    int remotePort;
    WiFiUDP udpModule;

    Com(int initialValue = LOCAL_UDP_PORT);
    int sendUdpMessage(char *msg, boolean broadcast = false);

public:
    static Com &getInstance();
    void initUDP();
    void cyclicHandleRxUDP();
    void sendAdcSensorDataUDP();
};

extern void sendWakeOnLan();

/* -- no code below this line */
#endif // _COM_H
/* ---END OF FILE --- */
