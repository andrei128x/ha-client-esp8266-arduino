#include "../system/system.h"
#if defined(ENABLE_MODULE_COM) && (ENABLE_MODULE_COM == true)
#include "../init/global.h"

/* FUNCTIONS unit */
#include "../devices/sensors.h"
#include "com.h"
#include "../devices\gate_controlller.h"
#include "../system/storage.h"

#include <WiFiUdp.h>
#include <ESP8266WiFi.h>

/* ----------- DEFINES ------------- */
#define DEV_WAKE_ON_LAN false

#define UDP_PORT 2001

/*------------ VARIABLES -------------- */
WiFiUDP udpModule;
#if defined(DEV_WAKE_ON_LAN) && (DEV_WAKE_ON_LAN == true)

byte fill[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
//byte mac[] = { 0x00, 0x1b, 0x38, 0x3d, 0xa5, 0x04 };
byte mac[] = {0x00, 0x19, 0x99, 0xff, 0x69, 0x2b};
#endif

static char incomingPacket[2048]; // buffer for incoming packets

char jsonTemplate[] = "{ \"C\":%ld, \"ADC0\":%d , \"ADC1\":%d, \"avg0\":%.3f, \"avg1\":%.3f, \"read0\":%d, \"read1\":%d, \"stableSig\":%d, \"gateState\":%d, \"RSSI\":%ld }";
char jsonData[300];

/* ----------- FUNCTIONS -------------- */

/* --- local functions --- */
void initCOM()
{
	char startUpTemplate[] = "[%s] Reboot counter: %d; using SSID/password: %s, %s";
	char startUpMsg[256];
	// set up UDP protocol
	udpModule.begin(40000);

	// getDataFromEEPROM(); already initialized
	sprintf(startUpMsg, startUpTemplate, global_host, u32ResetCounter, storedDataEEPROM.SSID, storedDataEEPROM.password);

	udpModule.beginPacket(LOCAL_CONTROLLER_ADDRESS, UDP_PORT);

	udpModule.write(startUpMsg, strlen(startUpMsg));
	udpModule.endPacket();

	u32ResetCounter++;
}

#if defined(DEV_WAKE_ON_LAN) && (DEV_WAKE_ON_LAN == true)
void sendWakeOnLan()
{
	udpModule.beginPacket("192.168.100.3", 0x69);
	udpModule.write(fill, 6);
	for (int i = 0; i < 16; i++)
	{
		udpModule.write(mac, 6);
	}
	udpModule.endPacket();
}
#endif

void cyclicHandleRxUDP()
{
	int packetSize = udpModule.parsePacket();
	if (packetSize)
	{
		// receive incoming UDP packets
		// Serial.printf("Received %d bytes from %s, port %d\n", packetSize, Udp.remoteIP().toString().c_str(), Udp.remotePort());
		int len = udpModule.read(incomingPacket, packetSize);
		if (len > 0)
		{
			incomingPacket[len] = 0;
		}
	}
}

void sendAdcSensorDataUDP()
{
	static char cycle = 0;
	static unsigned int localUdpPort = 4210; // local port to listen on // TODO move port ID somewhere else

	static unsigned int disconnectedFromWiFiCounter = 0;

	if (cycle == 0)
	{

		cycleHandleServo();	// TODO cycleHandleServo() <- find out if this is the correct place to call this function
		sprintf(jsonData, jsonTemplate, taskCnt, computedADC0, computedADC1, avg0, avg1, readVal0, readVal1, (int)(stableADC0 && stableADC1), (int)gateState, WiFi.RSSI());

		int result;
		if (WiFi.status() == WL_CONNECTED)
		{
			udpModule.beginPacket(LOCAL_CONTROLLER_ADDRESS, UDP_PORT);	// TODO save Forwarding UDP IP using the HTTP API dedicated for this
			// udpModule.write(jsonData.c_str(), jsonData.length());

			udpModule.write(jsonData, strlen(jsonData));
			result = udpModule.endPacket();
		}

		if (result)
			setActivityStateLED(ACTIVITY_START);
		else
			setActivityStateLED(ACTIVITY_STOPPED);

		// Serial.print(result);
		// Serial.print(" ");
		// Serial.print(WiFi.status());
		// Serial.print(" ");
	}

	cycle = (cycle + 1) % 71; // print every Nth task ~ 1 second
	disconnectedFromWiFiCounter++;
}

/* ---END OF FILE --- */
#endif