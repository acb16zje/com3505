// MyThing.ino
// COM3505 lab exercises sketch template
#include <SPI.h>
#include <WiFi.h>

uint64_t chipid;
byte mac[6];

const char* ssid = "VM5521249";
const char* password =  "mgvw7GcNckn5";

// initialisation entry point
void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(115200);
  
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi..");
  }

  Serial.println("Connected to the WiFi network");
  WiFi.macAddress(mac);
}

// task loop entry point
void loop() {
  digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(1000);                       // wait for a second
  digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
  delay(1000);                       // wait for a second

  chipid = ESP.getEfuseMac();                                    // The chip ID is not the MAC address(length: 6 bytes).
  Serial.printf("ESP32 Chip ID = %04X",(uint16_t)(chipid>>32));  // Print High 2 bytes
  Serial.printf("%08X\n",(uint32_t)chipid);                      // Print Low 4 bytes

  Serial.print("MAC: ");

  for (int i = 0; i < 6; i++) {
    if (i == 5) {
      Serial.println(mac[i], HEX);
    } else {
      Serial.print(mac[i], HEX);
      Serial.print(":");
    }
  }
  
  delay(3000);
}
