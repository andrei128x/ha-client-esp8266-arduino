/* FUNCTIONS unit */
#ifndef _GLOBAL_H
#define _GLOBAL_H

#include <Arduino.h>

/* ----------- DEFINES ------------- */

/* Activity LED state */
typedef enum
{
	ACTIVITY_NO_INIT,
	ACTIVITY_START,
	ACTIVITY_STOPPED
} activityState_type;

/* ----------- VARIABLES sections ------------- */

extern const char *global_host;		// set the OTA name for the ESP8266 module ()
extern const char *global_ssid;		// SSID used to connect as client to local WiFi
extern const char *global_password; // password used to connect to the local WiFi

extern const char *globalSelfSSID;		// SSID used by this device when connection to pre-defined AP fails
extern const char *globalSelfPassword;	// password this device accepts to be used

/* ----------- FUNCTIONS section -------------- */

/* -- no code below this line */
#endif
/* ---END OF FILE --- */
