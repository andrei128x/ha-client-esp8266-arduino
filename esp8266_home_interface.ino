#include "src/init/setup.h"
#include "src/system/state_machine.h"

/* ------------ SETUP FUNCTION --------------- */
void setup()
{
	// doTransition(SM_STATE_STARTUP_NO_WIFI);
	vDoInitSM();
}




/* ------------ MAIN FUNCTION ( WARNING: only NON-blocking function calls allowed! ) -------------- */
void loop()
{
	doExecute();
}

/* ----------------END APPLICATION -------------- */
