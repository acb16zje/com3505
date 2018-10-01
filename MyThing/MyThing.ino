// MyThing.ino
// COM3505 lab exercises sketch template
#include <SPI.h>
#include <WiFi.h>

uint64_t chipid;  
//char ssid[] = "eduroam";     // the name of your network
//int status = WL_IDLE_STATUS;     // the Wifi radio's status
//
//byte mac[6];                     // the MAC address of your Wifi shield

// initialisation entry point
void setup() {
  Serial.begin(115200);
//  pinMode(LED_BUILTIN, OUTPUT);

//  status = WiFi.begin(ssid);
}

// task loop entry point
void loop() {
  chipid=ESP.getEfuseMac();//The chip ID is essentially its MAC address(length: 6 bytes).
  Serial.printf("ESP32 Chip ID = %04X",(uint16_t)(chipid>>32));//print High 2 bytes
  Serial.printf("%08X\n",(uint32_t)chipid);//print Low 4bytes.

  delay(3000);
//  if ( status != WL_CONNECTED) { 
//    Serial.println("Couldn't get a wifi connection");
//    while(true);
//  } 
//  // if you are connected, print your MAC address:
//  else {
//    WiFi.macAddress(mac);
//    Serial.print("MAC: ");
//    Serial.print(mac[5],HEX);
//    Serial.print(":");
//    Serial.print(mac[4],HEX);
//    Serial.print(":");
//    Serial.print(mac[3],HEX);
//    Serial.print(":");
//    Serial.print(mac[2],HEX);
//    Serial.print(":");
//    Serial.print(mac[1],HEX);
//    Serial.print(":");
//    Serial.println(mac[0],HEX);
//  }
//  digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
//  delay(1000);                       // wait for a second
//  digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
//  delay(1000);                       // wait for a second
}
