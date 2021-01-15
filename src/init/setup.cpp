#include "./setup.h"
#include "../system/system.h"

/*------------ VARIABLES -------------- */
IPAddress myIP;

/* FUNCTIONS unit */
void globalInitNoWiFi()
{
	/* check for Warm Reset and update timestamp with Reset Safe's content */
	checkWarmFlag();
	// delay(1000);

	/* switch on led */
	pinMode(ONBOARD_LED, OUTPUT);
	digitalWrite(ONBOARD_LED, LOW);

	Serial.println("------------------------------------------------------");
	Serial.print("Last reset reason: ");
	Serial.println(ESP.getResetInfoPtr()->reason);
	Serial.println("------------------------------------------------------");

	Serial.println("Boot complete, waiting for WiFi connection...");
	// WiFi.disconnect();
	WiFi.mode(WIFI_STA);

	// FIXME FIX THE FUCKING WIFI issues; update - add AP config mode/SM

	initStorageEEPROM();
	WiFi.begin(global_ssid, global_password);
	//FIXME fix storage shit !!
	// WiFi.begin(storedDataEEPROM.SSID, storedDataEEPROM.password);

	Serial.println("Connecting to WiFi...");
	Serial.print("Using SSID: ");
	Serial.println(global_ssid);
	// Serial.println(storedDataEEPROM.SSID);
	// Serial.print(", length is ");
	// Serial.println(strlen(storedDataEEPROM.SSID));

	Serial.print("Using Password: ");
	Serial.println(global_password);
}

void globalInitWiFiAvailable()
{
	//FIXME - fix eeprom storage
	// Serial.println(storedDataEEPROM.password);
	// Serial.print(", length is ");
	// Serial.println(strlen(storedDataEEPROM.password));

	// boolean isIdentical = (strcmp(storedDataEEPROM.SSID, global_ssid) == 0) && (strcmp(storedDataEEPROM.password, global_password) == 0);
	// Serial.print("SSID and password comparison result: ");
	// Serial.println(isIdentical, DEC);

	// int8_t connectState = 0;

	/* switch off led */
	digitalWrite(ONBOARD_LED, HIGH);

	/* print local IP */

	myIP = WiFi.localIP();
	Serial.print("Obtained DHCP IP: ");
	Serial.println(myIP);

#if defined(DEV_SELF_ACCESS_POINT) && (DEV_SELF_ACCESS_POINT == true)
	// if (!connectState)
	{
		// WiFi.disconnect();
		// #error "section disabled"
		// initStorageEEPROM();
		getDataFromEEPROM();

		const char *ssidAP = globalSelfSSID;
		const char *passwordAP = globalSelfPassword;

		// WiFi.softAP(ssidAP, passwordAP);
		// IPAddress myIP = WiFi.softAPIP();
		Serial.print("AP IP address: ");
		Serial.println(myIP);
	}
#endif

	/* setup the OTA server */
	startOTA();

#if defined(ENABLE_MODULE_SENSORS_ONE_WIRE) && (ENABLE_MODULE_SENSORS_ONE_WIRE == true) // OneWire ENABLED
	/* start sensors */
	initTempSensor();
#endif

#if defined(ENABLE_MODULE_MOTOR) && (ENABLE_MODULE_MOTOR == true)
	/* init motor ESC */
	initMotorControl();
#endif

#if defined(ENABLE_MODULE_GATE_CONTROLLER) && (ENABLE_MODULE_GATE_CONTROLLER == true)
	/* init servo for the gate controller */
	initGateStates(); // TODO - investigate moving WIFI-independant code to globalInitNoWiFi()
#endif

	/* init the web server */
	initWebServer();

	/* init the SECURE RANDOM module */
	//void vInitADC();

#if defined(ENABLE_MODULE_SENSORS_ADS1x15) && (ENABLE_MODULE_SENSORS_ADS1x15 == true)
	/* init the ADC sensors */
	initCurrentSensorsADC(); // TODO - #2 as above
#endif

#if defined(ENABLE_MODULE_COM) && (ENABLE_MODULE_COM == true)
	// UDP and other stuff
	initCOM();
#endif

	/* CONFIG FINISHED */
	Serial.println("\nBoot finished");
	Serial.println("------------------------------------------------------");

	/* reset soft AP */
	//WiFi.softAPdisconnect(true);
	//WiFi.enableAP(false);
}

/* ---END OF FILE --- */