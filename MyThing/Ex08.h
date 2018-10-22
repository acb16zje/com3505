#ifndef LAB_08_H
#define LAB_08_H

// SSID and Password of your WiFi router
const char* clientSSID = "gakki gakki";

const int httpPort = 9191;
const char* host = "com3505.gate.ac.uk";
const String email = "zjeng1@sheffield.ac.uk";

String mac;
String url;

// Create a WiFi client
WiFiClient client;

void startWifi();

#endif
