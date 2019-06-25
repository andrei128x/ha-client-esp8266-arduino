/* FUNCTIONS unit */
#ifndef _GLOBAL_H
	#define _GLOBAL_H


/* ----------- DEFINES ------------- */

/* Activity LED state */
typedef enum {
	ACTIVITY_NO_INIT, ACTIVITY_START, ACTIVITY_STOPPED
} activityState_type;


/* ----------- VARIABLES sections ------------- */

extern const char* global_host;		// set the OTA name for the ESP8266 module ()
extern const char* global_ssid;		// SSID used to connect as client to local WiFi
extern const char* global_password;	// password used to connect to the local WiFi

/* ----------- FUNCTIONS section -------------- */


/* -- no code below this line */
#endif
 /* ---END OF FILE --- */
