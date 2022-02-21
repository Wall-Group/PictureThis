#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include "Arduino.h"

#define JOYSTICK_UP 41
#define JOYSTICK_DOWN 40
#define JOYSTICK_LEFT 39
#define JOYSTICK_RIGHT 38

/*  Connect 1 terminal to ground
    Connect the other terminal in parralel 
    to ground and the necessary pin */

void joystick_move_UP();
void joystick_move_DOWN();
void joystick_move_LEFT();
void joystick_move_RIGHT();
