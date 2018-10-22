// Ex04.ino

// initialisation entry point
void setup04() {
  setup01();
}

// task loop entry point
void loop04() {
  dbg(loopDBG, "ESP32 MAC = "); // print the...
  dln(loopDBG, MAC_ADDRESS);    //   ...ESP's "ID"
  blink(3);                     // blink the on-board LED...
  delay(1000);                  //   ...and pause
}
