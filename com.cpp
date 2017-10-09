/* FUNCTIONS unit */
#include "global.h"
#include "system.h"
#include "sensors.h"
#include "com.h"


/* ----------- DEFINES ------------- */
#define N_DIMMERS 3
#define HTTP_PORT 80

/*------------ VARIABLES -------------- */
int dimmer_pin[] = { 14, 5, 15 };

ESP8266WebServer server(HTTP_PORT);


/* ----------- FUNCTIONS -------------- */

/* --- local functions --- */
void onStartOTA()
{
	// switch off all the PWMs during upgrade
	//	for (int i = 0; i < N_DIMMERS; i++)
	//		analogWrite(dimmer_pin[i], 0);
	//analogWrite(led_pin,0);
}

void onEndOTA()
{
	// do a fancy thing with our board led at end
	for (int i = 0; i < 30; i++) {
		//analogWrite(led_pin,(i*100) % 1001);
		delay(50);
	}

}

void serverHandleRootURI()
{
	String response;
	updateTemp();
	response = "<html><body><h1>Temperature: ";
	response += temperatureCString;
	response += " &#176;C<br>Uptime: ";

	getSystemUptime(&response);

	response += "</h1><br><h3>Last reset type: ";
	u32AsciiToString(&response, u32ResetType);

	response += "</h3></body></html>";

	server.send(200, "text/html", response);
	setActivityStateLED(ACTIVITY_START);
}

/* --- EXTERN functions --- */
void startOTA(const char *host)
{

	/* configure dimmers, and OTA server events */
	analogWriteRange(1000);
	//analogWrite(led_pin,990);

	for (int i = 0; i < N_DIMMERS; i++) {
		pinMode(dimmer_pin[i], OUTPUT);
		analogWrite(dimmer_pin[i], 50);
	}

	ArduinoOTA.setHostname(host);
	ArduinoOTA.onStart(onStartOTA);

	ArduinoOTA.onEnd(onEndOTA);

	ArduinoOTA.onError([](ota_error_t error) {ESP.restart();});

	ArduinoOTA.begin();
}

void cyclicHandleOTA()
{
	ArduinoOTA.handle();
}

void initWebServer()
{
	server.on("/", serverHandleRootURI);
	server.begin();
	Serial.println("HTTP server started");
}

void cyclicHandleWebRequests()
{
	server.handleClient();
}

