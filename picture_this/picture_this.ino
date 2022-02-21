#include <Wire.h>
#include "SparkFun_Qwiic_Relay.h"
#include <Bounce.h>
#include "joystick.h"

#include <MatrixHardware_Teensy4_ShieldV5.h>
#include <SmartMatrix.h>

#define COLOR_DEPTH 24
const uint16_t kMatrixWidth = 32;
const uint16_t kMatrixHeight = 32;
const uint8_t kPanelType = SM_PANELTYPE_HUB75_32ROW_MOD16SCAN;

const int joystickUP_pin = 9;
const int joystickDOWN_pin = 10;
const int joystickLEFT_pin = 11;
const int joystickRIGHT_pin = 12;

unsigned int count = 0;            // how many times has it changed to low
unsigned long countAt = 0;         // when count changed
unsigned int countPrinted = 0;     // last count printed

Bounce joystickUP = Bounce(joystickUP_pin, 10);
Bounce joystickDOWN = Bounce(joystickDOWN_pin, 10);
Bounce joystickLEFT = Bounce(joystickLEFT_pin, 10);
Bounce joystickRIGHT = Bounce(joystickRIGHT_pin, 10);

const int RELAY_ADDR = 0x6D;
Qwiic_Relay qr(RELAY_ADDR);

void setup()
{
  Wire.begin();

  pinMode(joystickUP_pin, INPUT_PULLUP);
  pinMode(joystickDOWN_pin, INPUT_PULLUP);
  pinMode(joystickLEFT_pin, INPUT_PULLUP);
  pinMode(joystickRIGHT_pin, INPUT_PULLUP);

  Serial.begin(57600);
  while(!Serial);
  Serial.println("Joystick test:");
  
  if (!qr.begin()) {
    Serial.printf("Incompatible\n");
  } else {
    Serial.printf("Successful begin()\n");
  }
}

void loop() {
  
  if (joystickUP.update()) {
    if (joystickUP.fallingEdge()) {
       while (!joystickDOWN.update()){
        Serial.write("DOWN\n");
        delay(100);
      }
    }
  } 
  else if (joystickDOWN.update()) {
    if (joystickDOWN.fallingEdge()) {
      while (!joystickDOWN.update()){
        Serial.write("DOWN\n");
        delay(100);
      }
    }
  } 
  if (joystickLEFT.update()) {
    if (joystickLEFT.fallingEdge()) {
      Serial.write("LEFT\n");
    }
  } 
  else if (joystickRIGHT.update()) {
    if (joystickRIGHT.fallingEdge()) {
      Serial.write("RIGHT\n");
    }
  } 
}
