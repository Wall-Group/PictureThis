#include <Bounce.h>
#include "joystick.h"


const int joystickUP_pin = 9;
const int joystickDOWN_pin = 10;
const int joystickLEFT_pin = 11;
const int joystickRIGHT_pin = 12;

Bounce joystickUP = Bounce(joystickUP_pin, 10);
Bounce joystickDOWN = Bounce(joystickDOWN_pin, 10);
Bounce joystickLEFT = Bounce(joystickLEFT_pin, 10);
Bounce joystickRIGHT = Bounce(joystickRIGHT_pin, 10);

void setup() {
  pinMode(joystickUP_pin, INPUT_PULLUP);
  pinMode(joystickDOWN_pin, INPUT_PULLUP);
  pinMode(joystickLEFT_pin, INPUT_PULLUP);
  pinMode(joystickRIGHT_pin, INPUT_PULLUP);
  
  Serial.begin(57600);
  while(!Serial);
  Serial.println("Joystick test:");
}

unsigned int count = 0;            // how many times has it changed to low
unsigned long countAt = 0;         // when count changed
unsigned int countPrinted = 0;     // last count printed

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
