#include "joystick.h"

void setup() {
  /* May have to change HIGH to RISING */
  /* HIGH implies it will be called while the pin is high */
  attachInterrupt(digitalPinToInterrupt(JOYSTICK_UP), joystick_move_UP, HIGH);
  attachInterrupt(digitalPinToInterrupt(JOYSTICK_DOWN), joystick_move_DOWN, HIGH);
  attachInterrupt(digitalPinToInterrupt(JOYSTICK_LEFT), joystick_move_LEFT, HIGH);
  attachInterrupt(digitalPinToInterrupt(JOYSTICK_RIGHT), joystick_move_RIGHT, HIGH);

}

void loop() {
  // put your main code here, to run repeatedly:

}
