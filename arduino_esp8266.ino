#include "system.h"
#include "sensors.h"
#include "com.h"
#include "motor.h"

#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <ESP8266WebServer.h>

/*------------ MACROS -------------- */
#define SELF_ACCESS_POINT	false
#define USE_ACTIVITY_LED	false

#define ONBOARD_LED D4

/*------------ MACROS -------------- */

/*------------ VARIABLES -------------- */

const char* ssid = ".....";
const char* password = ".....";
const char* host = "OTA-LEDS";

#if defined(SELF_ACCESS_POINT) && (SELF_ACCESS_POINT == true)
const char *ssidAP = "ESPap";
const char *passwordAP = "thereisnospoon";
#endif

/* ------------ SETUP FUNCTION --------------- */
void setup()
{
	/* check for Warm Reset and update timestamp with Reset Safe's content */
	checkWarmFlag();

	Serial.begin(115200);

	/* switch on led */
	pinMode(ONBOARD_LED, OUTPUT);
	digitalWrite(ONBOARD_LED, LOW);

	Serial.println("Booting");
	WiFi.mode(WIFI_STA);

	WiFi.begin(ssid, password);

	while (WiFi.waitForConnectResult() != WL_CONNECTED) {
		WiFi.begin(ssid, password);
		Serial.println("Retrying connection...");
	}
	/* switch off led */
	digitalWrite(ONBOARD_LED, HIGH);

	startOTA(host);

	/* print local IP */
	IPAddress myIP = WiFi.localIP();
	Serial.println(myIP);

#if defined(SELF_ACCESS_POINT) && (SELF_ACCESS_POINT == true)
#error "section disabled"
	WiFi.softAP(ssidAP, passwordAP);
	IPAddress myIP = WiFi.softAPIP();
	Serial.print("AP IP address: ");
	Serial.println(myIP);
#endif

	/* setup the OTA server */
	startOTA(host);

	/* start sensors */
	initTempSensor();

	/* init motor ESC */
	initMotorControl();

	/* init the web server*/
	initWebServer();

	/* CONFIG FINISHED */
	Serial.println("Ready");

	/* reset soft AP */
	//WiFi.softAPdisconnect(true);
	//WiFi.enableAP(false);
}

/*------------ HELPER FUNCTIONS -------------- */

// task processing function (pseudo-scheduler)
void vDoHadleTasks()
{
	static boolean pin_level = LOW;
	static unsigned long saved_time = 0; // first task starts as soon as possible

	unsigned long current_time = millis();
	unsigned long delta = current_time - saved_time;

	// perform some activity every 100ms
	if (delta >= 100) {	//more than 100ms have passed
		saved_time = current_time;

#if defined(USE_ACTIVITY_LED) && (USE_ACTIVITY_LED == true)
		handleActivityLED();
#endif

		// send some LOG INFO over the (emulated) serial

		String timestamp = "";
		getSystemUptime(&timestamp);
		//Serial.println(timestamp);

		updateTemp();
		Serial.println(temperatureCString);

		updateMotorSpeed();
		//Serial.printf("%d ms\n",saved_time);
	}
}


/*------------ MAIN FUNCTION -------------- */
void loop()
{

	/* ---- mandatory section HANDLE of INCOMING DATA ------ */
	cyclicHandleOTA();
	cyclicHandleWebRequests();

	/* ---- customer section ----------*/
	vDoHadleTasks();

}

/* ----------------END APPLICATION -------------- */

