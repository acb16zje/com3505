// Misc.h
#ifndef MISC_H
#define MISC_H

#include <WiFi.h>                   // WiFi

// WiFi, MAC and IP helpers /////////////////////////////////////////////////
char MAC_ADDRESS[13]; // MAC addresses are 12 chars, plus the NULL terminator
void startWiFi();
void getMAC(char *);
String ip2str(IPAddress);                 // helper for printing IP addresses

// LED utilities, Push button, loop slicing /////////////////////////////////
void ledOn();
void ledOff();
void blink(int = 1, int = 300);
const int pushButton = 14;

#endif
