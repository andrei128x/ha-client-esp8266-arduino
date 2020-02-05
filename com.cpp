#include "system.h"
#if defined(ENABLE_MODULE_COM) && (ENABLE_MODULE_COM == true)

/* FUNCTIONS unit */
#include "global.h"
#include "system.h"
#include "sensors.h"
#include "com.h"
#include "gate_controlller.h"

#include <WiFiUdp.h>
#include <ESP8266WiFi.h>

/* ----------- DEFINES ------------- */
#define DEV_WAKE_ON_LAN false

#define HTTP_PORT 80
#define UDP_PORT 2001

/*------------ VARIABLES -------------- */
WiFiUDP udpModule;
#if defined(DEV_WAKE_ON_LAN) && (DEV_WAKE_ON_LAN == true)

byte fill[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
//byte mac[] = { 0x00, 0x1b, 0x38, 0x3d, 0xa5, 0x04 };
byte mac[] = {0x00, 0x19, 0x99, 0xff, 0x69, 0x2b};
#endif

ESP8266WebServer server(HTTP_PORT);
static char incomingPacket[2048]; // buffer for incoming packets

char* jsonTemplate = "{ \"C\":%ld, \"ADC0\":%d , \"ADC1\":%d, \"avg0\":%.3f, \"avg1\":%.3f, \"read0\":%d, \"read1\":%d, \"RSSI\":%ld }";
char jsonData[300];

/* ----------- FUNCTIONS -------------- */

/* --- local functions --- */
void initCOM()
{
	// set up UDP protocol
	udpModule.begin(40000);
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

void serverHandleRootURI()
{
	String response;

	response = "<!doctype html>\n\
<html lang=\"en\">\n\
<head>\n\
	<meta charset=\"utf-8\">\n\
	<title>Home IOT Project</title>\n\
	<meta name=\"description\" content=\"Home IOT Project\">\n\
	<meta name=\"author\" content=\"SitePoint\">\n\
</head>\n\
<body>\n\
	<h3>\n";

	response += "		<p>Temperature: ";
	response += temperatureCString;
	response += "&#176;C</p>\n";

	response += "		<p>Uptime: ";
	getSystemUptime(&response);
	response += "</p>\n";

	response += "		<p>Device IP: " + server.hostHeader() + "</p>\n";
	//u32AsciiToString(&response, server.global_hostHeader());

	response += "		<p>Last reset type: ";
	u32AsciiToString(&response, u32ResetType);

	response += "</p>\n		<p>Signal strength:" + String((int)WiFi.RSSI()) + "</p>\n	</h3>\n</body>\n</html>";

	server.send(200, "text/html", response);
	setActivityStateLED(ACTIVITY_START);
}

void serverHandleJsonRequest()
{
	String response;

	response = "{\"temperature\":\"";
	response += temperatureCString;
	response += "\", \"uptime\":\"";

	getSystemUptime(&response);

	response += "\", \"reset\":\"";
	response.concat(u32ResetType);
	response += "\"}";

	server.send(200, "application/json", response);
	setActivityStateLED(ACTIVITY_START);
}

#if defined(ENABLE_MODULE_GATE_CONTROLLER) && (ENABLE_MODULE_GATE_CONTROLLER == true)
void serverHandleServoClickRequest()
{
	doClickButton(15); // no. of cycles the button is kept pressed; 10ms or 100ms cycles
	server.send(200, "application/json", "{ \"response\":\"[OK]\" }");
}
#endif

void serverHandleSetForwardingIP()
{
}

/* init function for http microservice */
void initWebServer()
{
	/* information APIs */
	server.on("/info", serverHandleRootURI);
	server.on("/info.json", serverHandleJsonRequest);

#if defined(ENABLE_MODULE_UDP_ADC_FORWARD) && (ENABLE_MODULE_UDP_ADC_FORWARD == true)
	server.on("/set-forward-ip", serverHandleSetForwardingIP);
#endif

#if defined(ENABLE_MODULE_GATE_CONTROLLER) && (ENABLE_MODULE_GATE_CONTROLLER == true)
	/* servo position APIs */
	server.on("/servo/click", serverHandleServoClickRequest);
#endif

	server.begin();
	Serial.println("HTTP server started");
}

/* mandatory cycle function for http service */
void cyclicHandleWebRequests()
{
	server.handleClient();
}

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

void sendAdcSensorData()
{
	static char cycle = 0;
	static unsigned int localUdpPort = 4210; // local port to listen on

	static unsigned int idx = 0;

	if (cycle == 0)
	{
		// jsonData = "{ \"ADC0\":$1, \"ADC1\":$2, \"avg0\":$3, \"avg1\":$4, \"read0\":$5, \"read1\":$6 }";
		// //jsonData = "{ \"ADC0\":1, \"ADC1\":2, \"avg0\":3, \"avg1\":4, \"read0\":5, \"read1\":6 }";

		// jsonData.replace("$1", String(computedADC0, DEC));
		// jsonData.replace("$2", String(computedADC1, DEC));
		// jsonData.replace("$3", String((int)avg0, DEC));
		// jsonData.replace("$4", String((int)avg0, DEC));
		// jsonData.replace("$5", String(readVal0, DEC));
		// jsonData.replace("$6", String(readVal1, DEC));

		// jsonData.toCharArray(data, jsonData.length() + 1);

		// data[jsonData.length() + 1] = 0;

		sprintf(jsonData, jsonTemplate, taskCnt, computedADC0, computedADC1, avg0, avg1, readVal0, readVal1, WiFi.RSSI());

		int result;
		if (WiFi.status() == WL_CONNECTED)
		{
			udpModule.beginPacket("192.168.100.17", UDP_PORT);
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

	if (WiFi.status() != WL_CONNECTED)
	{
		WiFi.begin(global_ssid, global_password);
		Serial.println(".");
	}

	cycle = (cycle + 1) % 125; // print every Nth task
	idx++;
}

/* ---END OF FILE --- */
#endif