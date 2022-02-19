#include <avr/io.h>
#include <avr/interrupt.h>

#define JOYSTICK_UP 41
#define JOYSTICK_DOWN 40
#define JOYSTICK_LEFT 39
#define JOYSTICK_RIGHT 38

/*  Connect 1 terminal to ground
    Connecct the other terminal in parralel 
    to ground and the necessary pin */

joystick_move_UP();
joystick_move_DOWN();
joystick_move_LEFT();
joystick_move_RIGHT();