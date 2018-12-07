#ifndef ULTRA_S
#define ULTRA_S

#define frontTrigPin 26             // pin A0
#define frontEchoPin 25             // pin A1
#define backTrigPin 21              // GPIO 25, the pin below TX
#define backEchoPin 4               // pin A5

void startUltrasonicSensor();
short getFrontDistance();
short getBackDistance();

#endif
