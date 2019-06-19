/* FUNCTIONS unit */
#include "global.h"
#include "system.h"
#include "sensors.h"
#include "com.h"
#include "gate_controlller.h"

#include <WiFiUdp.h>


/* ----------- DEFINES ------------- */
#define DEV_WAKE_ON_LAN false

#define N_DIMMERS 3
#define HTTP_PORT 80

/*------------ VARIABLES -------------- */
int dimmer_pin[] = { 14, 5, 15 };

#if defined(DEV_WAKE_ON_LAN) && (DEV_WAKE_ON_LAN == true)
WiFiUDP Udp;

byte fill[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
//byte mac[] = { 0x00, 0x1b, 0x38, 0x3d, 0xa5, 0x04 };
byte mac[] = { 0x00, 0x19, 0x99, 0xff, 0x69, 0x2b };
#endif

ESP8266WebServer server(HTTP_PORT);


/* ----------- FUNCTIONS -------------- */

/* --- local functions --- */
#if defined(DEV_WAKE_ON_LAN) && (DEV_WAKE_ON_LAN == true)
void sendWakeOnLan()
{
	Udp.beginPacket("192.168.100.3", 0x69 );
	Udp.write(fill, 6);
	for(int i=0; i<16; i++)
	{
		Udp.write(mac, 6);
	}
  Udp.endPacket();
}
#endif

void serverHandleRootURI()
{
	String response;
	updateTemp();

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
	updateTemp();
	response = "{\"temperature\":\"";	response += temperatureCString;	response += "\", \"uptime\":\"";

	getSystemUptime(&response);

	response += "\", \"reset\":\""; u32AsciiToString(&response, u32ResetType); response += "\"}";

	server.send(200, "application/json", response);
	setActivityStateLED(ACTIVITY_START);
}

void serverHandleServoClickRequest()
{
	doClickButton(10);
	server.send(200, "application/json", "{ \"response\":\"[OK]\" }");
}


/* init function for http microservice */
void initWebServer()
{
	/* information APIs */
	server.on("/info", serverHandleRootURI);
	server.on("/info.json", serverHandleJsonRequest);

	/* servo position APIs */
	server.on("/servo/click", serverHandleServoClickRequest);

	server.begin();
	Serial.println("HTTP server started");
}

/* mandatory cycle function for http service */
void cyclicHandleWebRequests()
{
	server.handleClient();
}



/* --- hooks for OTA functions --- */
void onStartOTA()
{
	// switch off all the PWMs during upgrade
	//	for (int i = 0; i < N_DIMMERS; i++)
	//		analogWrite(dimmer_pin[i], 0);
	//analogWrite(led_pin,0);

	/* reset servo position */
	setServoPosition(0);
}

void onEndOTA()
{
	// do a fancy thing with our board led at end
	// for (int i = 0; i < 30; i++) {
	// 	//analogWrite(led_pin,(i*100) % 1001);
	// 	delay(50);
	// }

}


void startOTA(const char *host)
{
	ArduinoOTA.setHostname(host);
	//ArduinoOTA.setPassword((const char *)"1234");	// set password for OTA programming

	ArduinoOTA.onStart(onStartOTA);
	ArduinoOTA.onEnd(onEndOTA);
	ArduinoOTA.onError([](ota_error_t error) {ESP.restart();});

	ArduinoOTA.begin();
}




/* mandatory cyclic function for OTA reprogramming */
void cyclicHandleOTA()
{
	ArduinoOTA.handle();
}
