/* FUNCTIONS unit */
#ifndef _GLOBAL_H
	#define _GLOBAL_H


/* ----------- DEFINES ------------- */

/* Activity LED state */
typedef enum {
	ACTIVITY_NO_INIT, ACTIVITY_START, ACTIVITY_STOPPED
} activityState_type;


/* ----------- VARIABLES sections ------------- */

extern const char* global_host;
extern const char* global_ssid;
extern const char* global_password;

/* ----------- FUNCTIONS section -------------- */


/* -- no code below this line */
#endif
 /* ---END OF FILE --- */
