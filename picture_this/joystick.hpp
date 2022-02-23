#ifndef _JOYSTICK_H_
#define _JOYSTICK_H_

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include "Arduino.h"

namespace joystick {

#define JOYSTICK_UP 41
#define JOYSTICK_DOWN 40
#define JOYSTICK_LEFT 39
#define JOYSTICK_RIGHT 38

/*  Connect 1 terminal to ground
    Connect the other terminal in parralel 
    to ground and the necessary pin */

void Init();

void PollJoystick();

}  // namespace joystick
#endif  // _JOYSTICK_H_
