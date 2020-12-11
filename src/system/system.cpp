/* FUNCTIONS unit */
#include "../init/setup.h"
#include "./system.h"

/* ----------- DEFINES ------------- */
#define MS_TO_S (1000)
#define S_TO_MIN (60)
#define MIN_TO_H (60)
#define H_TO_DAY (24)

/*------------ VARIABLES -------------- */
activityState_type activityState = ACTIVITY_NO_INIT;
unsigned long u32WarmResetTimeDelta = 0;

unsigned long u32WarmResetPattern __attribute__((section(".noinit")));
unsigned long long u32TotalUptimeMS __attribute__((section(".noinit")));
unsigned long u32ResetCounter __attribute__((section(".noinit")));

unsigned long u32ResetType = *((int *)"DLOC"); /* defaults to COLD reset, reversed due to endianness */

unsigned long ulSysTaskCnt = 0;
/* ----------- FUNCTIONS -------------- */

/* --- uptime calculation --- */

void getSystemUptime(String *result)
{
	/* update the ResetSafe uptime counter */
	// unsigned long ms = millis();
	// u32TotalUptimeMS = ms;

	// ms += u32WarmResetTimeDelta;
	// u32TotalUptimeMS = ms;

	u32TotalUptimeMS = millis() + u32WarmResetTimeDelta;

	String seconds;
	String minutes;
	String hours;
	String days;

	days = String((unsigned long)(u32TotalUptimeMS / MS_TO_S / S_TO_MIN / MIN_TO_H / H_TO_DAY));
	hours = String((unsigned long)(u32TotalUptimeMS / MS_TO_S / S_TO_MIN / MIN_TO_H) % H_TO_DAY);
	minutes = String((unsigned long)(u32TotalUptimeMS / MS_TO_S / S_TO_MIN) % MIN_TO_H);
	seconds = String((unsigned long)(u32TotalUptimeMS / MS_TO_S) % S_TO_MIN);

	/* pad single digit with zeroes */

	if (days.length() < 2)
		days = "0" + days;

	if (hours.length() < 2)
		hours = "0" + hours;
	if (minutes.length() < 2)
		minutes = "0" + minutes;
	if (seconds.length() < 2)
		seconds = "0" + seconds;

	// https://en.wikipedia.org/wiki/ISO_8601#Time_intervals
	*result += "P"+days + "DT" + hours + "H" + minutes + "M" + seconds + "S";
}

/* set state for Activity LED behaviour */
void setActivityStateLED(activityState_type state)
{
	activityState = state;
}

/* --- activity indication led behaviour --- */
void handleActivityLED()
{

	const int MAXCOUNT = 10;
	static unsigned char pin_level = LOW; // active LOW
	static int counter = 0;

	// start blinking for a specific interval
	if (activityState == ACTIVITY_START)
	{
		if (counter < MAXCOUNT)
		{

			pin_level = 1 - pin_level;

			counter++;
		}
		else
		{ // interval passed, stop the blinking
			counter = 0;
			activityState = ACTIVITY_STOPPED;
			pin_level = HIGH;
		}
	}

	// Serial.print("LED");
	// Serial.print(pin_level);

	digitalWrite(ONBOARD_LED, (boolean)pin_level);
}

void checkWarmFlag()
{

	Serial.println(u32WarmResetPattern, HEX);

	/* WARM reset not found */
	if (u32WarmResetPattern != *((int *)"MRAW"))
	{
		u32WarmResetPattern = *((int *)"MRAW"); /* update pattern in ResetSafe to WARM, reversed due to endianness */
		u32TotalUptimeMS = 0;
		u32ResetCounter = 0;
	}
	else
	{ /* WARM reset found here */
		u32WarmResetTimeDelta = u32TotalUptimeMS;
		u32ResetType = u32WarmResetPattern;
	}
}

/* function converts an u32 containing 4 ASCII values to human readable for html output */
void u32AsciiToString(String *result, unsigned long value)
{
	*result += (char)(value >> 24 & 0xFF);
	*result += (char)(value >> 16 & 0xFF);
	*result += (char)(value >> 8 & 0xFF);
	*result += (char)(value & 0xFF);
}

/* ---END OF FILE --- */
