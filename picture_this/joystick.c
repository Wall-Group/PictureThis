#include "joystick.h"

joystick_move_UP() {
    Serial.write("Up");
}

joystick_move_DOWN() {
    Serial.write("Down");
}

joystick_move_LEFT() {
    Serial.write("Left");
}

joystick_move_RIGHT() {
    Serial.write("Right")
}