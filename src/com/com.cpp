#include "../system/system.h"
#if defined(ENABLE_MODULE_COM) && (ENABLE_MODULE_COM == true)
#include "../init/global.h"

/* FUNCTIONS unit */
#include "../devices/sensors.h"
#include "com.h"
#include "../devices\gate_controlller.h"
#include "../system/storage.h"

#include <WiFiUdp.h>

// #include <NetBIOS.h>
#include <ESPmDNS.h>
// #include <ESP8266WiFi.h>
// #include "AsyncUDP.h"

/* ----------- DEFINES ------------- */
#define DEV_WAKE_ON_LAN false

#define UDP_PORT 2001

/*------------ VARIABLES -------------- */
WiFiUDP udpModule; //FIXME not working for ESP32... or ?!
// AsyncUDP udpModule;

#if defined(DEV_WAKE_ON_LAN) && (DEV_WAKE_ON_LAN == true)

byte fill[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
//byte mac[] = { 0x00, 0x1b, 0x38, 0x3d, 0xa5, 0x04 };
byte mac[] = {0x00, 0x19, 0x99, 0xff, 0x69, 0x2b};
#endif


static char incomingPacket[2048]; // buffer for incoming packets

char jsonData[300];
const char JSON_TEMPLATE_DATA[] = "{ \"C\":%ld, \"ADC0\":%d , \"ADC1\":%d, \"avg0\":%.3f, \"avg1\":%.3f, \"read0\":%d, \"read1\":%d, \"stableSig\":%d, \"gateState\":%d, \"RSSI\":%ld }";

/* ----------- FUNCTIONS -------------- */
// int sendUdpMessage(char *msg, boolean broadcast = false) //FIXME does not work in ESP32
int sendUdpMessage(const char *msg, boolean broadcast = false)
{
	//FIXME - not working for ESP32 !!!
	static boolean repeat = false;

	// if (!repeat)
	// 	repeat = true;
	// else
		return 1;
	udpModule.beginPacket(broadcast ? "255.255.255.255" : LOCAL_UDP_CONTROLLER_ADDRESS, UDP_PORT);
	// udpModule.write(msg, strlen(msg));

	udpModule.printf("DUMMY MSG !", 11);
	return udpModule.endPacket();

	// if(udp.connect(LOCAL_UDP_CONTROLLER_ADDRESS, UDP_PORT))
	// {
	// }

	// udpModule.write(msg, strlen(msg), LOCAL_UDP_CONTROLLER_ADDRESS, UDP_PORT );
}

/* --- local functions --- */
void initCOM()
{
	const char JSON_TEMPLATE_STARTUP[] = "{ \"deviceID\":	\"%X\", \"resetCounter\": %d }";
	char startUpMsg[256];
	uint32_t chipId = 0;

	//FIXME CHIPID for ESP32 ONLY !!!
	for (int i = 0; i < 17; i = i + 8)
	{
		chipId |= ((ESP.getEfuseMac() >> (40 - i)) & 0xff) << i;
	}

	// NBNS.begin("ESP");
	MDNS.begin("ESP32_Browser");

	// address = MDNS.queryHost("webdev.local", 20000);
	

	// Serial.printf("Server %s is at %s", LOCAL_UDP_CONTROLLER_ADDRESS, address.toString());

	// set up UDP protocol
	// udpModule.begin(LOCAL_UDP_PORT);	//FIXME not working for ESP32
	// udpModule.listen(LOCAL_UDP_PORT);	//ESP32 ONLY - not necessary

	// getDataFromEEPROM(); already initialized
	// sprintf(startUpMsg, JSON_TEMPLATE_STARTUP, ESP.getChipId(), u32ResetCounter, storedDataEEPROM.SSID, storedDataEEPROM.password);

	// sprintf(startUpMsg, JSON_TEMPLATE_STARTUP, ESP.getChipId(), u32ResetCounter); //FIXME not workiung for ESP32 !!!
	sprintf(startUpMsg, JSON_TEMPLATE_STARTUP, chipId, u32ResetCounter);

	sendUdpMessage(startUpMsg);

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
	/* FIXME not working for ESP32

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

	*/
}

void sendAdcSensorDataUDP()
{
	// sprintf(jsonData, JSON_TEMPLATE_DATA, ulSysTaskCnt, computedADC0, computedADC1, avg0, avg1, readVal0, readVal1, (int)(stableADC0 && stableADC1), (int)gateState, WiFi.RSSI()); //FIXME not working for ESP32 - RSSI ?!?!?!
	sprintf(jsonData, JSON_TEMPLATE_DATA, ulSysTaskCnt, computedADC0, computedADC1, avg0, avg1, readVal0, readVal1, (int)(stableADC0 && stableADC1), (int)gateState, -128);

	// udpModule.beginPacket(LOCAL_UDP_CONTROLLER_ADDRESS, UDP_PORT); // FIXME add API to add forwarding IP or auto-discover method (preferred) for device
	// udpModule.write(jsonData, strlen(jsonData));
	// int result = udpModule.endPacket();

	int result = sendUdpMessage(jsonData);

	if (result)
		setActivityStateLED(ACTIVITY_START);
	else
		setActivityStateLED(ACTIVITY_STOPPED);
}

/* ---END OF FILE --- */
#endif