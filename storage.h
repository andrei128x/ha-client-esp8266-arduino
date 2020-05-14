/* FUNCTIONS unit */
#ifndef _STORAGE_H
#define _STORAGE_H

/* ----------- DEFINES ------------- */
struct storedDataEEPROM_struct
{
    char SSID[32];
    char password[64];
};

/* ----------- VARIABLES sections ------------- */
extern storedDataEEPROM_struct storedDataEEPROM;

/* ----------- FUNCTIONS section -------------- */
extern void initStorageEEPROM();
extern void getDataFromEEPROM();
extern void putDataToEEPROM();

/* -- no code below this line */
#endif
/* ---END OF FILE --- */
