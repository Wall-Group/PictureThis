# Picture This

## Table of Contents
1. [Overview](#Overview)
3. [UserStories](#User)
4. [Specifications](#Specifications)
5. [Diagrams](#Diagrams)

## Overview
### Description
A net 64x64 RGB LED matrix is mounted on the wall at roughly child-height (TBD). LEDs can be lit or dimmed using a cursor controlled by a joystick. The cursor can cycle between three sizes depending on user preference. Eight different colors can be chosen for each led (including off). A user can turn off all LEDs on the board using a single button.

### Instructions

1. Use the joystick to move the pen.
2. Set the color with one of the 8 available color buttons.
3. Change the pen size with the pen button.
4. Clear the screen with the reset button.

## UserStories
### Required User Stories

#### Designer can select one or more LEDs using the joystick
* Reason: The joystick allows the designer to create designs. If they couldn't use the joystick to select an LED, they would have no way of interacting with the display.

#### Designer can set the size of the cursor using a button
* Reason: Given the number of LEDs on the display, it might be tedious to light one LED at a time. Allowing the designer to select more than one LED simultaneously fosters a positive experience.

#### Designer can select/change LED color (including off)
* Reason: This is a multi-color display. Designers will want to use more than one color or turn off an LED depending on the desired design

#### Designer can see which LED is selected
* Reason: The designer needs to know which LED they are currently drawing on or setting the color for

#### Designed can erase the entire display at once
* Reason: Turning off each LED individually is tedious. An instant clean start is desirable in some scenarios

<br>

### Optional User Stories

#### Designer can play a game on the matrix
* Reason: This accepts a wider audience- instead of pure design, the display becomes interactive.

<br>

## Specifications

### Inputs
#### Physical joystick input [x1]
- Used to control LED cursor
- 4 GPIO pins
- 1 power pin

#### Color Buttons [x8]
- Used to set the color of the LEDs under the cursor
- White, Red, Green, Blue, Yellow, Orange, Purple, Clear

#### Reset button [x1]
- Sets all LEDs on the display to clear (off)

#### Cursor size button [x1]
- Cycles between 3 available cursor sizes

<br>

### Outputs
#### 64x64 LED Matrix
- Individual LEDs able to light up, multiple at a time
- Each LED can have one of 8 colors (including off)

#### Collection of blinking LEDs to show cursor
- The designer will use the cursor to set LEDs on the matrix

<br>

### Constraints
* Designer should not have access to internal wiring
    * This could damage the module and/or harm the designer
    
* Designer should not have direct access to the LED matrix
    * This could damage the matrix and/or harm the designer

* All parts of module should be durable and resistant to tampering and/or misuse

<br>

## Diagrams
### Block Diagram
<img src="https://i.imgur.com/ryA6fUi.png" width=500>
