// Ex01.ino

// initialisation entry point
void setup01() {
  Serial.begin(115200);
  pinMode(BUILTIN_LED, OUTPUT);

  getMAC(MAC_ADDRESS);
}

// task loop entry point
void loop01() {
  Serial.printf("ESP32 MAC = %s\n", MAC_ADDRESS);
  blink(3);
  delay(3000);
}

void getMAC(char* buf) {
  uint64_t mac = ESP.getEfuseMac(); // ...to string (high 2, low 4):
  char rev[13];
  sprintf(rev, "%04X%08X", (uint16_t) (mac >> 32), (uint32_t) mac);

  // the byte order in the ESP has to be reversed relative to normal Arduino
  for(int i = 0, j = 11; i < 18; i += 3, j -= 2) {
    buf[i] = rev[j - 1];
    buf[i + 1] = rev[j];
    buf[i + 2] = ':';
  }
  buf[17] = '\0';
}

void ledOn()  { digitalWrite(BUILTIN_LED, HIGH); }

void ledOff() { digitalWrite(BUILTIN_LED, LOW); }

void blink(int times, int pause) {
  ledOff();
  for(int i = 0; i < times; i++) {
    ledOn(); delay(pause); ledOff(); delay(pause);
  }
}
