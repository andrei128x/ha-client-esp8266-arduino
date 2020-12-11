/* STATE MACHINE unit */
#include "../init/setup.h"
#include "./state_machine.h"

/* ----------- DEFINES ------------- */

/* ----------- VARIABLES sections ------------- */
#define DBG(arg)                    \
    do                              \
    {                               \
        Serial.print("[");          \
        Serial.print(__FUNCTION__); \
        Serial.print("] ");         \
        Serial.println(arg);        \
    } while (0);

#define TRANSITION(current_state, next_state, update_state_func, enter_func)     \
    {                                                                            \
#current_state, current_state, next_state, update_state_func, enter_func \
    }

typedef struct
{
    char verbose_name[48];
    machine_states_type current_state;
    machine_states_type next_state;
    void (*update_state_func)();
    void (*enter_func)();
} transition_t;

transition_t transition_table[] = {
    TRANSITION(SM_STATE_NO_INIT, SM_STATE_STARTUP_NO_WIFI, update_SM_STATE_NO_INIT, NULL),
    TRANSITION(SM_STATE_STARTUP_NO_WIFI, SM_STATE_STARTUP_WIFI, update_SM_STATE_STARTUP_NO_WIFI, enter_SM_STATE_STARTUP_NO_WIFI),
    TRANSITION(SM_STATE_STARTUP_WIFI, SM_STATE_RUNNING_WIFI, NULL, enter_SM_STATE_STARTUP_WIFI),
    TRANSITION(SM_STATE_RUNNING_WIFI, SM_STATE_RUNNING_WIFI_DISCONNECTED, update_SM_STATE_RUNNING_WIFI, enter_SM_STATE_RUNNING_WIFI),
    TRANSITION(SM_STATE_RUNNING_WIFI_DISCONNECTED, SM_STATE_RUNNING_WIFI, update_SM_STATE_RUNNING_WIFI_DISCONNECTED, NULL),
};

current_state_t local_SM;

/* ----------- FUNCTIONS -------------- */
void vDoInitSM()
{
    Serial.begin(250000);
    delay(100);
    Serial.println("------------------------------------------------------");
    // Serial.print((String) "[" + __FUNCTION__ + "]");
    DBG("");
    strcpy(local_SM.name, transition_table[SM_STATE_NO_INIT].verbose_name);
    local_SM.sm_id = SM_STATE_NO_INIT;
    local_SM.init = false;
}

/* ------------ HELPER FUNCTIONS -------------- */
// task processing function (pseudo-scheduler)
inline void vDoHadlePeriodicTasksRunningWifi()
{

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
    //cycleHandleServo();
#endif

#if defined(ENABLE_MODULE_MOTOR) && (ENABLE_MODULE_MOTOR == true)
    updateMotorSpeed(); //motor features NOT integrated
#endif
    //Serial.printf("%d ms\n",u32LastExecutionTime);

    //sendWakeOnLan();

    //Serial.print("ADC value: ");
    //getSeed();

    updateCurrentSensorsADC();

    static char cycle = 0;

    if (cycle == 0)
        sendAdcSensorDataUDP();
    cycle = (cycle + 1) % 251; // print every Nth task ~ 1 second
}

void update_SM_STATE_NO_INIT()
{
    DBG("");
    doTransition(SM_STATE_STARTUP_NO_WIFI);
}

void enter_SM_STATE_STARTUP_NO_WIFI()
{
    DBG("");
    globalInitNoWiFi();
}

void update_SM_STATE_STARTUP_NO_WIFI()
{
    wl_status_t wifiState = WiFi.status();
    if (wifiState == WL_CONNECTED)
    {
        doTransition(SM_STATE_STARTUP_WIFI);
    }
}

void enter_SM_STATE_STARTUP_WIFI()
{
    DBG("");
    globalInitWiFiAvailable();
    doTransition(SM_STATE_RUNNING_WIFI);
}

void enter_SM_STATE_RUNNING_WIFI()
{
    DBG("!!!!!");
    // TODO broadcast info when network is available
}

void update_SM_STATE_RUNNING_WIFI()
{

    cyclicHandleOTA();
    cyclicHandleWebRequests();
    cyclicHandleRxUDP();

    vDoHadlePeriodicTasksRunningWifi();

    /* check WIFI connection and update accordingly */
    wl_status_t wifiState = WiFi.status();
    if (wifiState != WL_CONNECTED)
    {
        doTransition(SM_STATE_RUNNING_WIFI_DISCONNECTED);
    }
}

void update_SM_STATE_RUNNING_WIFI_DISCONNECTED()
{
    /* do nothing */
    wl_status_t wifiState = WiFi.status();
    if (wifiState == WL_CONNECTED)
    {
        doTransition(SM_STATE_RUNNING_WIFI);
    }
}

boolean isNetworkAvailable()
{
    // return sm.state == SM_STATE_WIFI_CONNECTED;
}

void doTransition(machine_states_type reqested_state)
{
    for (unsigned char idx = 0; idx < sizeof(transition_table) / sizeof(transition_t); idx++)
    {
        // if ((transition_table[idx].current_state == transition_table[local_SM.sm_id].current_state) &&
        //     (transition_table[idx].next_state == reqested_state))
        if (transition_table[idx].current_state == reqested_state)
        {
            /* update current state machine */
            local_SM.sm_id = idx;
            local_SM.init = false;
            strcpy(local_SM.name, transition_table[local_SM.sm_id].verbose_name);

            /* print logs */
            DBG(local_SM.name);
        }
    }
}

/**
 * @brief Executes the function associated with current machine state
 * 
 */
void doExecute()
{
    /* check if entry function for current SM state has been executed */
    if (!local_SM.init)
    {
        // execute init function only once
        local_SM.init = true;

        /* execute transition function */
        if (transition_table[local_SM.sm_id].enter_func != NULL)
        {
            /* call enter_ function for current SM state */
            DBG(local_SM.name);

            transition_table[local_SM.sm_id].enter_func();
        }
    }
    else
    {
        static unsigned long u32LastExecutionTime = 0; // first task starts as soon as possible

        unsigned long u32CurrentTaskEntryTime = micros();
        unsigned long delta = u32CurrentTaskEntryTime - u32LastExecutionTime;

        unsigned long u32CurrentTaskEndTime;
        float fCpuLoad;

        // static time_t dorel_last = 0;

        // save time before reading
        // SYS_START_MEASUREMENT();

        //more than TASK_CYCLIC_INTERVAL has passed since last execution
        if (delta >= TASK_CYCLIC_INTERVAL * 1000)
        {

            /* execute periodic function for current state */
            if (transition_table[local_SM.sm_id].update_state_func != NULL)
                transition_table[local_SM.sm_id].update_state_func();

            /* calculate CPU load here */
            u32CurrentTaskEndTime = micros();

            fCpuLoad = (u32CurrentTaskEndTime - u32CurrentTaskEntryTime) * 100 / (u32CurrentTaskEntryTime - u32LastExecutionTime);
            u32LastExecutionTime = u32CurrentTaskEntryTime;

            if (ulSysTaskCnt % 25 == 0)
            {
                // Serial.printf("CPU Load: %.2f \n", fCpuLoad);
            }
            // Serial.println(u8CpuLoad);

            //save time after reading
            // SYS_STOP_MEASUREMENT();

            //calculate time delta in microseconds
            //SYS_DBG_PRINT_MEASUREMENT();
            ulSysTaskCnt++;
        }
    }
}

/* ---END OF FILE --- */
