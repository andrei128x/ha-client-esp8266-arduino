/* SECURE RANDOM generator unit */
#ifndef _SECURE_RAND_H
	#define _SECURE_RAND_H

#include <Arduino.h>
#include "global.h"

/* ----------- DEFINES ------------- */

/* ----------- VARIABLES sections ------------- */
extern unsigned char values[1024];
extern unsigned short int idx;

/* ----------- FUNCTIONS section -------------- */
extern void vInitADC();
extern unsigned short getSeed();


/* -- no code below this line */
#endif
/* ---END OF FILE --- */
