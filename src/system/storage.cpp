#include "./system.h"
#if defined(ENABLE_MODULE_EEPROM) && (ENABLE_MODULE_EEPROM == true)

/* ----------- INCLUDES ------------- */
#include <EEPROM.h>
#include "./storage.h"

/* ----------- DEFINES ------------- */
#define EEPROM_DATA_START_OFFSET 0x0 // use offset 0 for FLASH-emulated EEPROM read/write operations

/*------------ VARIABLES -------------- */
storedDataEEPROM_struct storedDataEEPROM = {NULL, NULL};

/* ----------- FUNCTIONS -------------- */
void initStorageEEPROM()
{
    EEPROM.begin(256);
    EEPROM.get(EEPROM_DATA_START_OFFSET, storedDataEEPROM);
}

void getDataFromEEPROM()
{
    EEPROM.get(EEPROM_DATA_START_OFFSET, storedDataEEPROM);
}

void putDataToEEPROM()
{
    EEPROM.put(EEPROM_DATA_START_OFFSET, storedDataEEPROM);
    EEPROM.commit(); // flush data back to FLASH-emulated EEPROM
}

/* ---END OF FILE --- */
#endif