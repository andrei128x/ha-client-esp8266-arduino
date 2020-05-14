#include "setup.h"
#include "system.h"

/*------------ VARIABLES -------------- */
IPAddress myIP;

/* FUNCTIONS unit */
void globalInit()
{

	Serial.begin(250000);

	/* check for Warm Reset and update timestamp with Reset Safe's content */
	checkWarmFlag();
	delay(1000);

	/* switch on led */
	pinMode(ONBOARD_LED, OUTPUT);
	digitalWrite(ONBOARD_LED, LOW);

	Serial.println("------------------------------------------------------");
	Serial.print("Last reset reason: ");
	Serial.println(ESP.getResetInfoPtr()->reason);
	Serial.println("------------------------------------------------------");
	Serial.println("Booting, waiting for WiFi connection...");
	WiFi.mode(WIFI_STA);

	initStorageEEPROM();
	// WiFi.begin(global_ssid, global_password);
	// WiFi.begin(storedDataEEPROM.SSID, storedDataEEPROM.password);

	Serial.print("Using SSID: ");
	Serial.println(storedDataEEPROM.SSID);
	Serial.print("Using Password: ");
	Serial.println(storedDataEEPROM.password);

	int8_t connectState = 0;
	
	WiFi.disconnect();
	
	for (unsigned char connectionCounter = 0; (connectionCounter < 5) && !connectState; connectionCounter++)
	{
		connectState = (WiFi.waitForConnectResult(3000) == WL_CONNECTED);
		Serial.print("Connection state: ");
		Serial.println(connectState);
		WiFi.begin(storedDataEEPROM.SSID, storedDataEEPROM.password);
		Serial.print("Retrying counter: ");
		Serial.println(connectionCounter);
	}

	/* switch off led */
	digitalWrite(ONBOARD_LED, HIGH);

	/* print local IP */

	myIP = WiFi.localIP();
	Serial.print("Obtained DHCP IP: ");
	Serial.println(myIP);

#if defined(DEV_SELF_ACCESS_POINT) && (DEV_SELF_ACCESS_POINT == true)
	if (!connectState)
	{
		// WiFi.disconnect();
		// #error "section disabled"
		// initStorageEEPROM();
		getDataFromEEPROM();

		const char *ssidAP = globalSelfSSID;
		const char *passwordAP = globalSelfPassword;

		WiFi.softAP(ssidAP, passwordAP);
		IPAddress myIP = WiFi.softAPIP();
		Serial.print("AP IP address: ");
		Serial.println(myIP);
	}
#endif

	/* setup the OTA server */
	startOTA(global_host);

#if defined(ENABLE_MODULE_ONE_WIRE) && (ENABLE_MODULE_ONE_WIRE == true) // OneWire ENABLED
	/* start sensors */
	initTempSensor();
#endif

#if defined(ENABLE_MODULE_MOTOR) && (ENABLE_MODULE_MOTOR == true)
	/* init motor ESC */
	initMotorControl();
#endif

#if defined(ENABLE_MODULE_GATE_CONTROLLER) && (ENABLE_MODULE_GATE_CONTROLLER == true)
	/* init servo for the gate controller */
	initGateStates();
#endif

	/* init the web server */
	initWebServer();

	// UDP and other stuff
	initCOM();

	/* init the SECURE RANDOM module */
	//void vInitADC();

#if defined(ENABLE_MODULE_SENSORS) && (ENABLE_MODULE_SENSORS == true)
	/* init the ADC sensors */
	initCurrentSensorsADC();
#endif

	/* CONFIG FINISHED */
	Serial.println("\nBoot finished");
	Serial.println("------------------------------------------------------");

	/* reset soft AP */
	//WiFi.softAPdisconnect(true);
	//WiFi.enableAP(false);
}

/* ---END OF FILE --- */