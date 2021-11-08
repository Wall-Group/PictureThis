# Picture This

## Table of Contents
1. [Overview](#Overview)
2. [UserStories](#User)
3. [Specifications](#Specifications)
4. [Diagrams](#Diagrams)

## Overview
### Description
A net 64x64 RGB LED matrix is mounted on a wall at child-height. Each LED can be individually controlled by a user, who can light/dim a given LED using several different colors.

## UserStories
**Required User Stories**

* Designer can select an LED using the joystick
    * Reason: The joystick allows the designer to create designs. If they couldn't use the joystick to select an LED, they would have no way of interacting with the display.

* Designer can select/change LED color (including off)
    * Reason: This is a multi-color display. Designers will want to use more than one color or turn off an LED depending on the desired design

* Designer can see which LED is selected
    * Reason: The designer needs to know which LED they are currently drawing on or setting the color for

* Designed can erase the entire display at once
    * Reason: Turning off each LED individually is tedious. An instant clean start is desirable in some scenarios


**Optional User Stories**

* Designer can play a game on the matrix
    * Reason: This accepts a wider audience- instead of pure design, the display becomes interactive.
* *picks nose*
* *puches tommy*
* *hangs on module*

## Specifications

### Inputs
- Physical joystick input [x1]
    - Used to select LED
    - 4 GPIO pins, 1 power pin
- Buttons [x9]
    - 8 colors (WRGBYCOP), 1 reset button
    - 9 pins
- Toggle Switch [x1]
    - Individual module power
    - Later

### Outputs
- 64x64 LED Matrix
    - Individual LEDs able to light up, multiple at a time
    - Each LED can have one of 8 colors (including off)
- Blinking LED to indicate selected LED


### Constraints
* Designer should not have access to internal wiring
    * This could damage the module and/or harm the designer
    
* Designer should not have direct access to the LED matrix
    * This could damage the matrix and/or harm the designer

* All parts of module should be durable and resistant to tampering and/or misuse

## Diagrams
### Block Diagram
![](https://i.imgur.com/xCllZdY.png)
