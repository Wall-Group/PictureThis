#include <Wire.h>
#include "SparkFun_Qwiic_Relay.h"

const int RELAY_ADDR = 0x6D;
Qwiic_Relay qr(RELAY_ADDR);

void setup()
{
  Wire.begin();
  if (!quad_relay.begin()) {
    Serial.printf("Incompatible\n");
  } else {
    Serial.printf("Successful begin()\n");
  }
}

void loop()
{

}
