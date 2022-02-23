#include <Wire.h>
#include "SparkFun_Qwiic_Relay.h"
#include "joystick.hpp"

#include <MatrixHardware_Teensy4_ShieldV5.h>
#include <SmartMatrix.h>

#define COLOR_DEPTH 24
const uint16_t kMatrixWidth = 32;
const uint16_t kMatrixHeight = 32;
const uint8_t kPanelType = SM_PANELTYPE_HUB75_32ROW_MOD16SCAN;

const int RELAY_ADDR = 0x6D;
Qwiic_Relay qr(RELAY_ADDR);

void setup()
{
  Wire.begin();

  Serial.begin(57600);
  while(!Serial);
  Serial.println("Joystick test:");
  
  if (!qr.begin()) {
    Serial.printf("Incompatible\n");
  } else {
    Serial.printf("Successful begin()\n");
  }

  qr.turnRelayOn(1);
}

void loop() {
  

}
