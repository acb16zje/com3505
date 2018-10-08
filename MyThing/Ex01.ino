// MyThing.ino
// COM3505 lab exercises sketch template
#include <SPI.h>
#include <WiFi.h>

uint64_t chipid;                    // 6 bytes = 48 bits wide, need at least 64 bits to fit in
byte mac[6];

// initialisation entry point
void setup() {
  Serial.begin(115200);
  pinMode(pushButton, INPUT_PULLUP);
  pinMode(externalLED, OUTPUT);
  
  WiFi.begin();
  WiFi.macAddress(mac);
}

// task loop entry point
void loop() {
  digitalWrite(pushButton, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(1000);                      // wait for a second
  digitalWrite(pushButton, LOW);    // turn the LED off by making the voltage LOW
  delay(1000);                      // wait for a second

  Serial.print("MAC: ");

  // Serial.println(mac[i]); will print out the MAC as decimal value
  // MAC Address is usually displayed in hexadecimal values
  // Serial.println(mac[i], HEX); will print out the MAC as hexadecimal value
  for (int i = 0; i < 6; i++) {
    if (i == 5) {
      Serial.println(mac[i], HEX);
    } else {
      Serial.print(mac[i], HEX);
      Serial.print(":");
    }
  }

  // Serial.printf() only prints up to uint32 (according to Arduino forum)
  chipid = ESP.getEfuseMac();                        // The chip ID is the reverse of the MAC address(length: 6 bytes), reason unknown yet
  Serial.printf("ESP32 Chip ID = %4X", chipid>>32);  // chipid needs to be shifted right by 32 bits to print the first 2 bytes of the MAC
  Serial.printf("%8X\n", chipid);                    // print the last 4 bytes of the MAC

  delay(3000);
}
