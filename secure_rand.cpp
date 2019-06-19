/* FUNCTIONS unit */
/* PURPOSE: seed generator, based on the ESP8266's ADC */
#include "secure_rand.h"

#define ONBOARD_LED D4
#define ONBOARD_SEED_PIN D1

/*------------ VARIABLES -------------- */
unsigned char values[1024];
unsigned short int idx = 0;

unsigned int ones=0, zeroes=0;

unsigned short computedVal = 0;

/* ----------- FUNCTIONS -------------- */

/* --- ADC init --- */
void vInitADC(){
    pinMode(ONBOARD_SEED_PIN, INPUT);
}


/* --- seed gerator function, based on the ESP8266's ADC --- */
unsigned short getSeed(){
    //unsigned short analogVal = (unsigned short)analogRead(ONBOARD_ADC) & B1;   // use only last bit
    unsigned short analogVal = (unsigned short)digitalRead(ONBOARD_SEED_PIN) & B1;   // use only last bit

    computedVal = (analogVal & ~(1<<idx)) | analogVal << idx;
    
    computedVal?ones++:zeroes++;
    //Serial.print("analogVal: ");
    //Serial.println(analogVal);

    //Serial.print("Computed: ");
    //Serial.println(computedVal, BIN);

    //Serial.print("Index: ");
    //Serial.println(idx);

    Serial.print(zeroes); Serial.print(" --- "); Serial.println(ones);

    idx +=1 ;
    idx %= 16;  // modulo index for shift operation

    return(analogVal);
}




/* ---END OF FILE --- */
