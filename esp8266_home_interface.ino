#include "setup.h"


/*------------ MACROS -------------- */
#define DEV_SELF_ACCESS_POINT false

/*------------ MACROS -------------- */
#define TASK_CYCLIC_INTERVAL 3 // milliseconds

/*------------ VARIABLES -------------- */
#if defined(DEV_SELF_ACCESS_POINT) && (DEV_SELF_ACCESS_POINT == true)
const char *ssidAP = "ESPap";
const char *passwordAP = "thereisnospoon";
#endif


/* ------------ SETUP FUNCTION --------------- */
void setup()
{
	globalInit();
}


/* ------------ HELPER FUNCTIONS -------------- */
// task processing function (pseudo-scheduler)
void vDoHadleTasks()
{
	//static boolean pin_level = LOW;
	static unsigned long u32LastExecutionTime = 0; // first task starts as soon as possible

	unsigned long u32CurrentTaskEntryTime = millis();
	unsigned long delta = u32CurrentTaskEntryTime - u32LastExecutionTime;

	unsigned long u32CurrentTaskEndTime;
	unsigned char u8CpuLoad;

	// static time_t dorel_last = 0;

	// save time before reading
	// SYS_START_MEASUREMENT();

	// perform some activity every 10ms
	if (delta >= TASK_CYCLIC_INTERVAL)
	{ //more than 10ms have passed
		u32LastExecutionTime = u32CurrentTaskEntryTime;

		/* EXPERIMENTAL CPU LOAD measuring
		time_t dorel_now = (time_t)(system_get_time());
		Serial.print("Clock ticks passed: ");
		Serial.print((dorel_now - dorel_last));
		dorel_last = dorel_now;
	*/

#if defined(USE_ACTIVITY_LED) && (USE_ACTIVITY_LED == true)
		handleActivityLED();
#endif

		// send some LOG INFO over the (emulated) serial

		String timestamp = "";
		getSystemUptime(&timestamp);
		//Serial.println(timestamp);

#if defined(ENABLE_MODULE_ONE_WIRE) && (ENABLE_MODULE_ONE_WIRE == true)
		updateTemp();
		Serial.print("Temperature: ");
		Serial.println(temperatureCString);
#endif

#if defined(ENABLE_MODULE_GATE_CONTROLLER) && (ENABLE_MODULE_GATE_CONTROLLER == true)
		/* update servo state machine */
		cycleHandleServo();
#endif

#if defined(ENABLE_MODULE_MOTOR) && (ENABLE_MODULE_MOTOR == true)
		updateMotorSpeed(); //motor features NOT integrated
#endif
		//Serial.printf("%d ms\n",u32LastExecutionTime);

		//sendWakeOnLan();

		//Serial.print("ADC value: ");
		//getSeed();

		updateCurrentSensorsADC();
		sendAdcSensorData();

		/* calculate CPU load here */
		u32CurrentTaskEndTime = millis();
		u8CpuLoad = (u32CurrentTaskEndTime - u32CurrentTaskEntryTime) / TASK_CYCLIC_INTERVAL; //calculate

		// Serial.print("CPU Load: ");
		// Serial.println(u8CpuLoad);

		//save time after reading
		// SYS_STOP_MEASUREMENT();

		//calculate time delta in microseconds
		//SYS_DBG_PRINT_MEASUREMENT();
		taskCnt++;
	}
}


/* ------------ MAIN FUNCTION -------------- */
void loop()
{

	/* ---- mandatory section HANDLE of INCOMING DATA ------ */
	cyclicHandleOTA();
	cyclicHandleWebRequests();
	cyclicHandleRxUDP();

	/* ---- customer section ---------- */
	vDoHadleTasks();
}


/* ----------------END APPLICATION -------------- */
