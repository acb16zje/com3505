#ifndef LAB_01_H
#define LAB_01_H

// MAC address ///////////////////////////////////////////////////////////////
uint64_t chipid;   // 6 bytes = 48 bits wide, need at least 64 bits to fit in
char MAC_ADDRESS[18]; // MAC addresses are 12 chars, plus the NULL terminator
void getMAC(char *);

// LED utilities /////////////////////////////////////////////////////////////
void ledOn();
void ledOff();
void blink(int = 1, int = 300);

#endif
