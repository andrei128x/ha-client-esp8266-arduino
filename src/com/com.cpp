#include "../system/system.h"
#if defined(ENABLE_MODULE_COM) && (ENABLE_MODULE_COM == true)
#include "../init/global.h"

/* FUNCTIONS unit */
#if defined(ENABLE_MODULE_SENSORS_CURRENT) && (ENABLE_MODULE_SENSORS_CURRENT == true)
#include "../devices/sensors_current.h"
#endif

#if defined(ENABLE_MODULE_SENSORS_ONE_WIRE_TEMP) && (ENABLE_MODULE_SENSORS_ONE_WIRE_TEMP == true)
#include "../devices/sensors_temp.h"
#endif

#include "com.h"
#include "../devices/gate_controlller.h"
#include "../system/storage.h"

#include <WiFiUdp.h>
#include <ESP8266WiFi.h>

/* ----------- DEFINES ------------- */
#define DEV_WAKE_ON_LAN false

/*------------ VARIABLES -------------- */

#if defined(DEV_WAKE_ON_LAN) && (DEV_WAKE_ON_LAN == true)

byte fill[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
// byte mac[] = { 0x00, 0x1b, 0x38, 0x3d, 0xa5, 0x04 };
byte mac[] = {0x00, 0x19, 0x99, 0xff, 0x69, 0x2b};
#endif

static char incomingPacket[2048]; // buffer for incoming packets

char jsonData[300];
const char jsonTemplate[] = "{ \"C\":%ld, \"ADC0\":%d , \"ADC1\":%d, \"avg0\":%.3f, \"avg1\":%.3f, \"read0\":%d, \"read1\":%d, \"stableSig\":%d, \"gateState\":%d, \"RSSI\":%ld }";

char startUpMsg[256];
const char startUpTemplate[] = "{ \"reboot\": \"[%s] Reboot counter: %d; using SSID/password: %s, %s\"}";

// Constructor
Com::Com(int initialValue) : remotePort(initialValue)
{
	u32ResetCounter++;
};

Com &Com::getInstance()
{
	static Com instance; // Initialized only once
	return instance;
}

void Com::initUDP()
{
	if (!initialized)
	{
		// set up UDP protocol
		udpModule.begin(LOCAL_UDP_PORT);

		// getDataFromEEPROM(); already initialized
		sprintf(startUpMsg, startUpTemplate, global_host, u32ResetCounter, "storedDataEEPROM.SSID", "storedDataEEPROM.password");
		Serial.println(startUpMsg);

		sendUdpMessage(startUpMsg);

		initialized = true;
	}
}

int Com::sendUdpMessage(char *msg, boolean broadcast)
{
	udpModule.beginPacket(broadcast ? "255.255.255.255" : SERVER_UDP_CONTROLLER_ADDRESS, SERVER_UDP_PORT);
	udpModule.write(msg, strlen(msg));
	return udpModule.endPacket();
};

/* function triggered frim thge task scheduler */
void Com::cyclicHandleRxUDP()
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

/* trigger transmission of UDP data */
void Com::sendAdcSensorDataUDP()
{
	static char cycle = 0;

	if (cycle == 0)
	{

#if defined(ENABLE_MODULE_GATE_CONTROLLER) && (ENABLE_MODULE_GATE_CONTROLLER == true)
		cycleHandleServo(); // TODO cycleHandleServo() <- find out if this is the correct place to call this function
		sprintf(jsonData, jsonTemplate, ulSysTaskCnt, computedADC0, computedADC1, avg0, avg1, readVal0, readVal1, (int)(stableADC0 && stableADC1), (int)gateState, WiFi.RSSI());
#endif

		// udpModule.beginPacket(SERVER_UDP_CONTROLLER_ADDRESS, UDP_PORT); // FIXME add API to add forwarding IP or auto-discover method (preferred) for device
		// udpModule.write(jsonData, strlen(jsonData));
		// int result = udpModule.endPacket();


		sprintf(jsonData, "{ \"data\": %s }", temperatureCString); // TODO add sequence counter into the JSON
		int result = sendUdpMessage(jsonData);

		if (result)
			setActivityStateLED(ACTIVITY_START);
		else
			setActivityStateLED(ACTIVITY_STOPPED);
	}

	// cycle = (cycle + 1) % 251; // print every Nth task
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

/* ---END OF FILE --- */
#endif