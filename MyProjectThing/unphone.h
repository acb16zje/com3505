// unphone.h
// core definitions and includes

#ifndef UNPHONE_H
#define UNPHONE_H

#include <Wire.h>               // I²C comms on the Arduino
#include <IOExpander.h>         // unPhone's IOExpander (controlled via I²C)
#include <Adafruit_Sensor.h>    // base class etc. for sensor abstraction

#ifdef UNPHONE_RCSWITCH
#include <RCSwitch.h>
#endif

// macros for debug calls to Serial.printf, with (D) and without (DD)
// new line, and to Serial.println (DDD)
extern bool UNPHONE_DBG;     // debug switch
#define D(args...)   if(UNPHONE_DBG) { Serial.printf(args); Serial.println(); }
#define DD(args...)  if(UNPHONE_DBG) Serial.printf(args);
#define DDD(s)       if(UNPHONE_DBG) Serial.println(s);

// power management chip API /////////////////////////////////////////////////
extern byte BM_I2Cadd;          // the chip lives here on I²C
extern byte BM_Watchdog;        // charge termination/timer control register
extern byte BM_OpCon;           // misc operation control register
extern byte BM_Status;          // system status register
extern byte BM_Version;         // vender / part / revision status register
bool checkPowerSwitch();        // returns status of USB power (true = on)
void setShipping(bool value);   // tells power management chip to shut down
void setRegister(byte address, byte reg, byte value);   //
byte getRegister(byte address, byte reg);               // I²C...
void write8(byte address, byte reg, byte value);        // ...helpers
byte read8(byte address, byte reg);                     //

#endif
