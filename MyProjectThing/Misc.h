#ifndef MISC_H
#define MISC_H

void ledOn();
void ledOff();
void blink(int = 1, int = 300);

String ip2str(IPAddress);                 // helper for printing IP addresses

#endif