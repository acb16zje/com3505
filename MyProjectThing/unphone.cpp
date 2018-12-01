// unphone.cpp
// core library

#include "unphone.h"

bool UNPHONE_DBG = false; // debug switch

// power management chip API /////////////////////////////////////////////////
byte BM_I2Cadd   = 0x6b;  // the chip lives here on I²C
byte BM_Watchdog = 0x05;  // charge termination/timer control register
byte BM_OpCon    = 0x07;  // misc operation control register
byte BM_Status   = 0x08;  // system status register
byte BM_Version  = 0x0a;  // vender / part / revision status register

bool checkPowerSwitch() {
  uint8_t inputPwrSw = IOExpander::digitalRead(IOExpander::POWER_SWITCH);

  bool usbPowerOn = // bit 2 of status register indicates if USB connected
    bitRead(getRegister(BM_I2Cadd, BM_Status), 2);

  if(!inputPwrSw) {  // when power switch off
    if(!usbPowerOn) { // and usb unplugged we go into shipping mode
      setShipping(true);
    } else { // power switch off and usb plugged in we sleep
      esp_sleep_enable_timer_wakeup(1000000); // sleep time is in uSec
      esp_deep_sleep_start();
    }
  }

  return usbPowerOn;
}

// ask BM chip to shutdown or start up
void setShipping(bool value) {
  byte result;
  if(value) {
    result=getRegister(BM_I2Cadd, BM_Watchdog);  // state of timing register
    bitClear(result, 5);                         // clear bit 5...
    bitClear(result, 4);                         // and bit 4 to disable...
    setRegister(BM_I2Cadd, BM_Watchdog, result); // WDT (REG05[5:4] = 00)

    result=getRegister(BM_I2Cadd, BM_OpCon);     // operational register
    bitSet(result, 5);                           // set bit 5 to disable...
    setRegister(BM_I2Cadd, BM_OpCon, result);    // BATFET (REG07[5] = 1)
  } else {
    result=getRegister(BM_I2Cadd, BM_Watchdog);  // state of timing register
    bitClear(result, 5);                         // clear bit 5...
    bitSet(result, 4);                           // and set bit 4 to enable...
    setRegister(BM_I2Cadd, BM_Watchdog, result); // WDT (REG05[5:4] = 01)

    result=getRegister(BM_I2Cadd, BM_OpCon);     // operational register
    bitClear(result, 5);                         // clear bit 5 to enable...
    setRegister(BM_I2Cadd, BM_OpCon, result);    // BATFET (REG07[5] = 0)
  }
}

// I2C helpers to drive the power management chip
// TODO share with IOExp versions?
void setRegister(byte address, byte reg, byte value) {
  write8(address, reg, value);
}
byte getRegister(byte address, byte reg) {
  byte result;
  result=read8(address, reg);
  return result;
}
void write8(byte address, byte reg, byte value) {
  Wire.beginTransmission(address);
  Wire.write((uint8_t)reg);
  Wire.write((uint8_t)value);
  Wire.endTransmission();
}
byte read8(byte address, byte reg) {
  byte value;
  Wire.beginTransmission(address);
  Wire.write((uint8_t)reg);
  Wire.endTransmission();
  Wire.requestFrom(address, (byte)1);
  value = Wire.read();
  Wire.endTransmission();
  return value;
}

// the accelerometer (with a unique ID) /////////////////////////////////////
Adafruit_LSM303_Accel_Unified accel = Adafruit_LSM303_Accel_Unified(54321);

// the LCD and touch screen /////////////////////////////////////////////////
#define TFT_DC   33
Adafruit_HX8357 tft =
  Adafruit_HX8357(IOExpander::LCD_CS, TFT_DC, IOExpander::LCD_RESET);
Adafruit_STMPE610 ts = Adafruit_STMPE610(IOExpander::TOUCH_CS);
// calibration data for converting raw touch data to the screen coordinates
#define TS_MINX 3800
#define TS_MAXX 100
#define TS_MINY 100
#define TS_MAXY 3750

// the VS1053 music and synth chip ///////////////////////////////////////////
Adafruit_VS1053_FilePlayer musicPlayer = Adafruit_VS1053_FilePlayer(
  IOExpander::MUSIC_RESET, IOExpander::MUSIC_CS, IOExpander::MUSIC_DCS,
  VS1053_DREQ, IOExpander::SD_CS
);

// the mic, and the I²S bus /////////////////////////////////////////////////
// I²S bus, used by the microphone, see e.g.:
// http://esp-idf.readthedocs.io/en/latest/api/peripherals/i2s.html
void i2s_config() {
  // input
  i2s_config_t i2s_in_config = {
    mode: (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
    sample_rate: 44100,
    bits_per_sample: (i2s_bits_per_sample_t) 32,
    channel_format: I2S_CHANNEL_FMT_RIGHT_LEFT,
    communication_format: (i2s_comm_format_t)
      (I2S_COMM_FORMAT_I2S | I2S_COMM_FORMAT_I2S_MSB),
    intr_alloc_flags: ESP_INTR_FLAG_LEVEL1,
    dma_buf_count: 14,
    dma_buf_len: 64
  };
  i2s_pin_config_t i2s_in_pin_config = {
    bck_io_num: MIC_BCLK,
    ws_io_num: MIC_LRCL,
    data_out_num: -1, //Not used
    data_in_num: MIC_DOUT
  };

  pinMode(MIC_DOUT, INPUT);  // aka pin TX connected to Data OUT (DOUT)
  pinMode(MIC_BCLK, OUTPUT); // aka RX to Bit Clock (BCLK) aka Clock
  pinMode(MIC_LRCL, OUTPUT); // aka A5 to Left Right Clock (LRCL) /Word Select

  i2s_driver_install((i2s_port_t)0, &i2s_in_config, 0, NULL);
  i2s_set_pin((i2s_port_t)0, &i2s_in_pin_config);
}
uint32_t read_i2s() {
  uint32_t sample_val[2] = {0, 0};
  uint8_t bytes_read = i2s_pop_sample(
    (i2s_port_t) 0, (char *) sample_val, portMAX_DELAY
  );
  return sample_val[0] << 5;
}

// the LoRa board and TTN LoRaWAN ///////////////////////////////////////////
// LoRaWAN NwkSKey, network session key
// this is the default Semtech key, which is used by the prototype TTN
// network initially
static const PROGMEM u1_t NWKSKEY[16] = {       // TODO change
  0xC7, 0xFC, 0xF2, 0xF1, 0xAC, 0x8D, 0x07, 0x09, 0x4F, 0x33, 0xD4,
  0xAA, 0xDE, 0x67, 0x17, 0xA9
};

// LoRaWAN AppSKey, application session key
// this is the default Semtech key, which is used by the prototype TTN
// network initially
static const u1_t PROGMEM APPSKEY[16] = {       // TODO change
  0xBE, 0xEF, 0xA3, 0xB7, 0x59, 0xAA, 0x79, 0x49, 0xC6, 0x06, 0x11,
  0x90, 0xAE, 0xEB, 0x47, 0x8A
};

// LoRaWAN end-device address (DevAddr)
// See http://thethingsnetwork.org/wiki/AddressSpace
static const u4_t DEVADDR = 0x260115CE;         // TODO change

// these callbacks are only used in over-the-air activation, so they are
// left empty here (we cannot leave them out completely unless
// DISABLE_JOIN is set in config.h, otherwise the linker will complain)
void os_getArtEui (u1_t* buf) { }
void os_getDevEui (u1_t* buf) { }
void os_getDevKey (u1_t* buf) { }

uint8_t mydata[] = "Hello, World!";
int loops=1;
uint8_t loopcount[] = " Loops: ";
osjob_t sendjob;

// schedule TX every this many seconds (might become longer due to duty
// cycle limitations).
const unsigned TX_INTERVAL = 60;

// pin mapping
const lmic_pinmap lmic_pins = {
  .nss = IOExpander::LORA_CS,
  .rxtx = LMIC_UNUSED_PIN,
  .rst = IOExpander::LORA_RESET,
  .dio = {39, 34, LMIC_UNUSED_PIN}  // modified to suit connection on unphone
};

void onEvent(ev_t ev) {
  Serial.print(os_getTime());
  Serial.print(": ");
  switch(ev) {
    case EV_SCAN_TIMEOUT:
      Serial.println(F("EV_SCAN_TIMEOUT"));
      break;
    case EV_BEACON_FOUND:
      Serial.println(F("EV_BEACON_FOUND"));
      break;
    case EV_BEACON_MISSED:
      Serial.println(F("EV_BEACON_MISSED"));
      break;
    case EV_BEACON_TRACKED:
      Serial.println(F("EV_BEACON_TRACKED"));
      break;
    case EV_JOINING:
      Serial.println(F("EV_JOINING"));
      break;
    case EV_JOINED:
      Serial.println(F("EV_JOINED"));
      break;
    case EV_RFU1:
      Serial.println(F("EV_RFU1"));
      break;
    case EV_JOIN_FAILED:
      Serial.println(F("EV_JOIN_FAILED"));
      break;
    case EV_REJOIN_FAILED:
      Serial.println(F("EV_REJOIN_FAILED"));
      break;
    case EV_TXCOMPLETE:
      Serial.println(F("EV_TXCOMPLETE (includes waiting for RX windows)"));
      if(LMIC.dataLen) {
        // data received in rx slot after tx
        Serial.print(F("Data Received: "));
        Serial.write(LMIC.frame+LMIC.dataBeg, LMIC.dataLen);
        Serial.println();
      }
      // schedule next transmission
      os_setTimedCallback(
        &sendjob, os_getTime()+sec2osticks(TX_INTERVAL), lmic_do_send
      );
      break;
    case EV_LOST_TSYNC:
      Serial.println(F("EV_LOST_TSYNC"));
      break;
    case EV_RESET:
      Serial.println(F("EV_RESET"));
      break;
    case EV_RXCOMPLETE:
      // data received in ping slot
      Serial.println(F("EV_RXCOMPLETE"));
      break;
    case EV_LINK_DEAD:
      Serial.println(F("EV_LINK_DEAD"));
      break;
    case EV_LINK_ALIVE:
      Serial.println(F("EV_LINK_ALIVE"));
      break;
     default:
      Serial.println(F("Unknown event"));
      break;
  }
}

void lmic_do_send(osjob_t* j) {
  // check if there is not a current TX/RX job running
  if(LMIC.opmode & OP_TXRXPEND) {
    D("OP_TXRXPEND, not sending");
  } else {
    // prepare upstream data transmission at the next possible time
    LMIC_setTxData2(1, mydata, sizeof(mydata)-1, 0);
    loopcount[0]=+1;
    LMIC_setTxData2(1, loopcount, sizeof(loopcount)-1, 0);
    D("Packet queued");
  }
  // next TX is scheduled after TX_COMPLETE event
}

void lmic_init() {
  // LMIC init
  Serial.println("doing os_init()...");
  if(testScreenActive)
    tft.println("LoRa Sending");
  os_init(); // if lora fails then will stop, allowing panic mess to be seen

  if(testScreenActive) {
    tft.fillScreen(HX8357_WHITE);
    TestScreen::init();
    tft.setCursor(65, 270);
    tft.setTextColor(HX8357_MAGENTA);
    tft.println("LoRa Sent");
  }

  // reset the MAC state; session and pending data transfers will be discarded
  Serial.println("doing LMIC_reset()...");
  LMIC_reset();

  // set static session parameters instead of dynamically establishing session
  // by joining the network, precomputed session parameters are provided
  #ifdef PROGMEM
  // on AVR, these values are stored in flash and only copied to RAM
  // once; copy them to a temporary buffer here, LMIC_setSession will
  // copy them into a buffer of its own again
  uint8_t appskey[sizeof(APPSKEY)];
  uint8_t nwkskey[sizeof(NWKSKEY)];
  memcpy_P(appskey, APPSKEY, sizeof(APPSKEY));
  memcpy_P(nwkskey, NWKSKEY, sizeof(NWKSKEY));
  Serial.println("doing LMIC_setSession() PROGMEM...");
  LMIC_setSession (0x1, DEVADDR, nwkskey, appskey);
  #else
  // if not running an AVR with PROGMEM, just use the arrays directly
  Serial.println("doing LMIC_setSession()...");
  LMIC_setSession (0x1, DEVADDR, NWKSKEY, APPSKEY);
  #endif

  // Set up the channels used by the Things Network, which correspond
  // to the defaults of most gateways. Without this, only three base
  // channels from the LoRaWAN specification are used, which certainly
  // works, so it is good for debugging, but can overload those
  // frequencies, so be sure to configure the full frequency range of
  // your network here (unless your network autoconfigures them).
  // Setting up channels should happen after LMIC_setSession, as that
  // configures the minimal channel set. All g-band except the last
  // (which is g2-band):
  LMIC_setupChannel(0, 868100000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);
  LMIC_setupChannel(1, 868300000, DR_RANGE_MAP(DR_SF12, DR_SF7B), BAND_CENTI);
  LMIC_setupChannel(2, 868500000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);
  LMIC_setupChannel(3, 867100000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);
  LMIC_setupChannel(4, 867300000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);
  LMIC_setupChannel(5, 867500000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);
  LMIC_setupChannel(6, 867700000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);
  LMIC_setupChannel(7, 867900000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);
  LMIC_setupChannel(8, 868800000, DR_RANGE_MAP(DR_FSK,  DR_FSK),  BAND_MILLI);

  // TTN defines an additional channel at 869.525Mhz using SF9 for class B
  // devices' ping slots. LMIC does not have an easy way to define set this
  // frequency and support for class B is spotty and untested, so this
  // frequency is not configured here.
  // Disable link check validation
  LMIC_setLinkCheckMode(0);
  // set data rate and transmit power (note: txpow seems to be ignored by lib)
  LMIC_setDrTxpow(DR_SF7, 14);
}
