#include <Wire.h>
#include <MatrixHardware_Teensy4_ShieldV5.h>
#include <SmartMatrix.h>
#include <Bounce.h>
#include <arduino-timer.h>
#include "SparkFun_Qwiic_Relay.h"

#include <string>

// COMMENT THIS OUT BEFORE SHIPPING. Controls debug prints
#define DEBUG

/*****************************************************************************/
/*      THIS CODE COPIED FROM SMART MATRIX LIBRARY FOR INITIALIZATION        */
/*****************************************************************************/
#define COLOR_DEPTH 24                  // known working: 24, 48 - If the sketch uses type `rgb24` directly, COLOR_DEPTH must be 24
const uint16_t kMatrixWidth = 64;        // known working: 32, 64, 96, 128
const uint16_t kMatrixHeight = 64;       // known working: 16, 32, 48, 64
const uint8_t kRefreshDepth = 36;       // known working: 24, 36, 48
const uint8_t kDmaBufferRows = 4;       // known working: 2-4, use 2 to save memory, more to keep from dropping frames and automatically lowering refresh rate
const uint8_t kPanelType = SMARTMATRIX_HUB75_32ROW_MOD16SCAN;   // use SMARTMATRIX_HUB75_16ROW_MOD8SCAN for common 16x32 panels
const uint32_t kMatrixOptions = (SMARTMATRIX_OPTIONS_NONE);      // see http://docs.pixelmatix.com/SmartMatrix for options
const uint8_t kBackgroundLayerOptions = (SM_BACKGROUND_OPTIONS_NONE);
const uint8_t kIndexedLayerOptions = (SM_INDEXED_OPTIONS_NONE);

SMARTMATRIX_ALLOCATE_BUFFERS(matrix, kMatrixWidth, kMatrixHeight, kRefreshDepth, kDmaBufferRows, kPanelType, kMatrixOptions);
SMARTMATRIX_ALLOCATE_BACKGROUND_LAYER(backgroundLayer, kMatrixWidth, kMatrixHeight, COLOR_DEPTH, kBackgroundLayerOptions);

// Blink the cursor every x
auto timer = timer_create_default();

struct rgb24 dots[64][64];

/*****************************************************************************/
/*                      CUSTOM GLOBALS FOR PICTURE THIS                      */
/*****************************************************************************/

/**************************************/
/*               Colors               */
/**************************************/
// Used to update current_color
const struct rgb24 kWhite = {0xff, 0xff, 0xff};
const struct rgb24 kRed = {0xff, 0x00, 0x00};
const struct rgb24 kGreen = {0x00, 0xff, 0x00};
const struct rgb24 kBlue = {0x00, 0x00, 0xff};
const struct rgb24 kYellow = {0xff, 0xff, 0x00};
const struct rgb24 kOrange = {0xff, 0xA5, 0x00};
const struct rgb24 kPurple = {0x80, 0x00, 0x80};
const struct rgb24 kClear = {0x00, 0x00, 0x00};
const struct rgb24 kClearBlink = {0x50, 0x50, 0x50};

// Holds the color being drawn. Initialized to no value / black / clear
struct rgb24 current_color = kGreen;


/**************************************/
/*            Drawing State           */
/**************************************/
// Center values for cursor
const int kCenterX = 32;
const int kCenterY = 32;

// Holds the cursor size
struct {
    int x = kCenterX;  // X position of center
    int y = kCenterY;  // Y position of center
    int r = 1;  // Radius of cursor
    int px = kCenterX;
    int py = kCenterY;
    int pr = r;
} cursor;

// Whether or not the user is currently drawing
bool drawing = false;

// True if cursor currently drawn. Used to blink when not drawing
bool blink = false;


/**************************************/
/*            Matrix Settings         */
/**************************************/
// Screen brightness. Change kPercent to change brightness
const int kPercent = 40;  // 50 = 50%, 25 = 25%, etc.
const int kBrightness = (kPercent * 255) / 100;  // <-- Don't change this


/**************************************/
/*             Bounce Setup           */
/**************************************/
// Const values to use for milliseconds argument to Bounce() object constructor
const int kBounceBtn   = 50;  // For buttons
const int kBounceStick = 50;  // For joystick

// How fast to move the cursor, in ms, when the joystick is held in a direction
// const int kDrawSpeed = 500; // Disallow for now...

// Const values to use for color button pins
const int kWhitePin  = 41;  // TODO: Select pins
const int kBluePin   = 40;
const int kGreenPin  = 39;
const int kRedPin    = 37;
const int kErasePin  = 36;
const int kPurplePin = 35;
const int kOrangePin = 34;
const int kYellowPin = 33;

// Const values to use for functionality button pins
const int kCursorPin = 31;
const int kResetPin  = 30;
const int kDrawPin   = 29;  // TODO: Select pins

// Const values to use for joystick pins
const int kStickUpPin = 27;  // TODO: Select pins
const int kStickDnPin = 28;
const int kStickLtPin = 24;
const int kStickRtPin = 25;

// Bounce objects for color buttons
Bounce btn_white  = Bounce(kWhitePin,  kBounceBtn);
Bounce btn_red    = Bounce(kRedPin,    kBounceBtn);
Bounce btn_green  = Bounce(kGreenPin,  kBounceBtn);
Bounce btn_blue   = Bounce(kBluePin,   kBounceBtn);
Bounce btn_yellow = Bounce(kYellowPin, kBounceBtn);
Bounce btn_orange = Bounce(kOrangePin, kBounceBtn);
Bounce btn_purple = Bounce(kPurplePin, kBounceBtn);
Bounce btn_erase  = Bounce(kErasePin,  kBounceBtn);

// Bounce objects for module functionality
Bounce btn_draw   = Bounce(kDrawPin,   kBounceBtn);
Bounce btn_reset  = Bounce(kResetPin,  kBounceBtn);
Bounce btn_cursor = Bounce(kCursorPin, kBounceBtn);

// Bounce objects for the joystick
Bounce stick_up = Bounce(kStickUpPin, kBounceStick);
Bounce stick_dn = Bounce(kStickDnPin, kBounceStick);
Bounce stick_lt = Bounce(kStickLtPin, kBounceStick);
Bounce stick_rt = Bounce(kStickRtPin, kBounceStick);


/**************************************/
/*           Relay Settings           */
/**************************************/
// The I2C address of the Qwiic relay
const int kRelayAddr = 0x6D;

// Used to command the Qwiic relay via I2C
Qwiic_Relay relay(kRelayAddr);

// The relay used to supply power to the top matrix
const int kTopMatrix = 1;

// The relay used to supply power to the bottom matrix
const int kBottomMatrix = 2;



/*****************************************************************************/
/*                    CUSTOM FUNCTIONS FOR PICTURE THIS                      */
/*****************************************************************************/
/// \brief Wrapper around Serial.printf
/// \remarks Constant formatted output. Guarded by preprocessor symbol
void Debug(std::string msg) {
#ifdef DEBUG
    Serial.printf(("[DEBUG]: " + msg + "\n").c_str());
#endif
}

struct rgb24 AccessDots(int x, int y) {
  int bounded_x, bounded_y;
  
  if (x > 63) bounded_x = 0;
  else if (x < 0) bounded_x = 63;
  else bounded_x = x;

  if (y > 63) bounded_y = 0;
  else if (y < 0) bounded_y = 63;
  else bounded_y = y;

  return dots[bounded_x][bounded_y];
}

void SetDots(int x, int y, struct rgb24 color) {
	int bounded_x, bounded_y;
  
	if (x > 63) bounded_x = 0;
	else if (x < 0) bounded_x = 63;
	else bounded_x = x;

	if (y > 63) bounded_y = 0;
	else if (y < 0) bounded_y = 63;
	else bounded_y = y;

	dots[bounded_x][bounded_y] = color;
}

void RecordLast() {
    blink = false;
    // cursor.pr = cursor.r;
    cursor.py = cursor.y;
    cursor.px = cursor.x;
}

void Blink() {
    // If not drawing, blink cursor

    struct rgb24 blink_color;
    // if (blink) blink_color = kClear
    struct rgb24 dot_color = AccessDots(cursor.x,cursor.y);

    if (blink && !SameColor(dot_color, current_color)) blink_color = AccessDots(cursor.x,cursor.y);
    else if (blink && !SameColor(current_color, kClear)) blink_color = kClear;
    else if (blink) blink_color = kClearBlink;
    else blink_color = current_color;

    backgroundLayer.fillCircle(
            cursor.x, cursor.y, cursor.r, blink_color);

    backgroundLayer.swapBuffers();
    blink = !blink;
}

void InitDots() {

    for (int i=0; i<64; i++) {
        for (int j=0; j<64; j++) {
            SetDots(i, j,  kClear);
        }
    }

}

bool SameColor(struct rgb24 a, struct rgb24 b) {
    bool r = a.red == b.red;
    bool g = a.green == b.green;
    bool bl = a.blue == b.blue;
    return r && g && bl;
}

void SetDot(int radius, int x, int y, struct rgb24 color) {

    if (radius == 1) {
        SetDots(x, y, color);
    }
    else if (radius == 2) {
        SetDots(x - 1, y - 1, color);
        SetDots(x, y - 1, color);
        SetDots(x + 1, y - 1, color);

        SetDots(x-1, y, color);
        SetDots(x, y, color);
        SetDots(x+1, y, color);

        SetDots(x-1, y+1, color);
        SetDots(x, y+1, color);
        SetDots(x+1, y+1, color);
    }
    else if (radius == 3) {
        // Top three dots
        SetDots(x-1, y-2, color);
        SetDots(x, y-2, color);
        SetDots(x+1, y-2, color);

        // 2nd row five dots
        SetDots(x-2, y-1, color);
        SetDots(x - 1, y - 1, color);
        SetDots(x, y - 1, color);
        SetDots(x + 1, y - 1, color);
        SetDots(x + 2, y-1, color);

        // Middle row five dots
        SetDots(x - 2, y, color);
        SetDots(x-1, y, color);
        SetDots(x, y, color);
        SetDots(x+1, y, color);
        SetDots(x+2, y, color);

        // 4th row five dots
        SetDots(x-2, y+1, color);
        SetDots(x-1, y+1, color);
        SetDots(x, y+1, color);
        SetDots(x+1, y+1, color);
        SetDots(x+2, y+1, color);

        // Bottom three dots
        SetDots(x-1, y+2, color);
        SetDots(x, y+2, color);
        SetDots(x+1, y+2, color);
    }
}


void DrawDot(int x, int y) {
    backgroundLayer.drawPixel(x, y, AccessDots(x,y));
    backgroundLayer.swapBuffers();
}

void DrawSquare(int x, int y) {
    // 2nd row five dots
    backgroundLayer.drawPixel(x - 1, y - 1, AccessDots(x-1,y-1));
    backgroundLayer.drawPixel(x, y - 1, AccessDots(x,y-1));
    backgroundLayer.drawPixel(x + 1, y - 1, AccessDots(x+1,y-1));

    // Middle row five dots
    backgroundLayer.drawPixel(x-1, y, AccessDots(x-1,y));
    backgroundLayer.drawPixel(x, y, AccessDots(x,y));
    backgroundLayer.drawPixel(x+1, y, AccessDots(x+1,y));

    // 4th row five dots
    backgroundLayer.drawPixel(x-1, y+1, AccessDots(x-1,y+1));
    backgroundLayer.drawPixel(x, y+1, AccessDots(x,y+1));
    backgroundLayer.drawPixel(x+1, y+1, AccessDots(x+1,y+1));

    backgroundLayer.swapBuffers();
}

void DrawCross(int x, int y) {
     // Top three dots
    backgroundLayer.drawPixel(x-1, y-2, AccessDots(x-1,y-2));
    backgroundLayer.drawPixel(x, y-2, AccessDots(x,y-2));
    backgroundLayer.drawPixel(x+1, y-2, AccessDots(x+1,y-2));

    // 2nd row five dots
    backgroundLayer.drawPixel(x-2, y-1, AccessDots(x-2,y-1));
    backgroundLayer.drawPixel(x - 1, y - 1, AccessDots(x-1,y-1));
    backgroundLayer.drawPixel(x, y - 1, AccessDots(x,y-1));
    backgroundLayer.drawPixel(x + 1, y - 1, AccessDots(x+1,y-1));
    backgroundLayer.drawPixel(x + 2, y-1, AccessDots(x+2,y-1));

    // Middle row five dots
    backgroundLayer.drawPixel(x - 2, y, AccessDots(x-2,y));
    backgroundLayer.drawPixel(x-1, y, AccessDots(x-1,y));
    backgroundLayer.drawPixel(x, y, AccessDots(x,y));
    backgroundLayer.drawPixel(x+1, y, AccessDots(x+1,y));
    backgroundLayer.drawPixel(x+2, y, AccessDots(x+2,y));

    // 4th row five dots
    backgroundLayer.drawPixel(x-2, y+1, AccessDots(x-2,y+1));
    backgroundLayer.drawPixel(x-1, y+1, AccessDots(x-1,y+1));
    backgroundLayer.drawPixel(x, y+1, AccessDots(x,y+1));
    backgroundLayer.drawPixel(x+1, y+1, AccessDots(x+1,y+1));
    backgroundLayer.drawPixel(x+2, y+1, AccessDots(x+2,y+1));

    // Bottom three dots
    backgroundLayer.drawPixel(x-1, y+2, AccessDots(x-1,y+2));
    backgroundLayer.drawPixel(x, y+2, AccessDots(x,y+2));
    backgroundLayer.drawPixel(x+1, y+2, AccessDots(x+1,y+2));

    backgroundLayer.swapBuffers();
}

void ResetPrevious(bool c = false) {
    
    int radius = c ? cursor.pr : cursor.r;

    // Use the previous radius to determine which pixels to reset
    switch (radius) {
        case 1:
            DrawDot(cursor.px, cursor.py);
            break;
        case 2:
            DrawSquare(cursor.px, cursor.py);
            break;
        case 3:
            DrawCross(cursor.px, cursor.py);
            break;
    }

}

/*****************************************************************************/
/*                                  SETUP                                    */
/*****************************************************************************/
void setup()
{
    Wire.begin();

    // Set Bounce object pins to INPUT_PULLUP, as suggested by the library
    pinMode(kWhitePin,  INPUT_PULLUP);
    pinMode(kRedPin,    INPUT_PULLUP);
    pinMode(kGreenPin,  INPUT_PULLUP);
    pinMode(kBluePin,   INPUT_PULLUP);
    pinMode(kYellowPin, INPUT_PULLUP);
    pinMode(kOrangePin, INPUT_PULLUP);
    pinMode(kPurplePin, INPUT_PULLUP);
    pinMode(kErasePin,  INPUT_PULLUP);

    // Set Bounce object pins to INPUT_PULLUP for joystick
    pinMode(kStickUpPin, INPUT_PULLUP);
    pinMode(kStickDnPin, INPUT_PULLUP);
    pinMode(kStickLtPin, INPUT_PULLUP);
    pinMode(kStickRtPin, INPUT_PULLUP);

    // Set Bounce object pints to INPUT_PULLUP for other buttons
    pinMode(kDrawPin, INPUT_PULLUP);
    pinMode(kResetPin, INPUT_PULLUP);
    pinMode(kCursorPin, INPUT_PULLUP);

    InitDots();

    matrix.addLayer(&backgroundLayer);
    matrix.begin();
    matrix.setBrightness(kBrightness);

    timer.every(250, Blink);
}



/*****************************************************************************/
/*                                  LOOP                                     */
/*****************************************************************************/
void loop() {
    timer.tick();
    
    /**************************************/
    /*            Button Polling          */
    /**************************************/
    if (btn_white.update() && btn_white.fallingEdge()) {
        current_color = kWhite;
    }
    else if (btn_red.update() && btn_red.fallingEdge()) {
        current_color = kRed;
    }
    else if (btn_green.update() && btn_green.fallingEdge()) {
        current_color = kGreen;
    }
    else if (btn_blue.update() && btn_blue.fallingEdge()) {
        current_color = kBlue;
    }
    else if (btn_yellow.update() && btn_yellow.fallingEdge()) {
        current_color = kYellow;
    }
    else if (btn_orange.update() && btn_orange.fallingEdge()) {
        current_color = kOrange;
    }
    else if (btn_purple.update() && btn_purple.fallingEdge()) {
        current_color = kPurple;
    }
    else if (btn_erase.update() && btn_erase.fallingEdge()) {
        current_color = kClear;
    }

    if (btn_draw.update() && btn_draw.fallingEdge()) {
        drawing = !drawing;
    }
    
    if (btn_reset.update() && btn_reset.fallingEdge()) {
        backgroundLayer.fillScreen(kClear);
        backgroundLayer.swapBuffers();

        drawing = false;

        cursor.r = 1;
        cursor.x = kCenterX;
        cursor.y = kCenterY;

        RecordLast();
        InitDots();

    }

    if (btn_cursor.update() && btn_cursor.fallingEdge()) {
        timer.cancel();

        // Make sure cursor is NOT drawn before changing size
        switch (cursor.r) {
            case 1:
                DrawDot(cursor.x, cursor.y);
                break;
            case 2:
                DrawSquare(cursor.x, cursor.y);
                break;
            case 3:
                DrawCross(cursor.x, cursor.y);
                break;
        }

        cursor.pr = cursor.r;
        cursor.r = (cursor.r % 3) + 1;  // Allow radii 1,2,3
        ResetPrevious(true);
        timer.every(250, Blink);
    }



    /**************************************/
    /*           Joystick Polling         */
    /**************************************/
    bool moved = false;
    if (stick_up.update() && stick_up.fallingEdge()) {
        RecordLast();
        cursor.y += 1;
        moved = true;
        if (cursor.y > 63) cursor.y = 0;  // wrap around
    }

    else if (stick_dn.update() && stick_dn.fallingEdge()) {
        RecordLast();
        cursor.y -= 1;
        moved = true;
        if (cursor.y < 0) cursor.y = 63;  // wrap around
    }
    
    else if (stick_lt.update() && stick_lt.fallingEdge()) {
        RecordLast();
        cursor.x -= 1;
        moved = true;
        if (cursor.x < 0) cursor.x = 63;  // wrap around
    }
    
    else if (stick_rt.update() && stick_rt.fallingEdge()) {
        RecordLast();
        cursor.x += 1;
        moved = true;
        if (cursor.x > 63) cursor.x = 0;  // wrap around
    }


    /**************************************/
    /*               Drawing              */
    /**************************************/
    struct rgb24 draw_color = (SameColor(current_color, kClear)) ? kClearBlink : current_color;
    if (moved && drawing) {
        SetDot(cursor.r, cursor.x, cursor.y, current_color);
        ResetPrevious();
        backgroundLayer.fillCircle(
                    cursor.x, cursor.y, cursor.r, draw_color);
        backgroundLayer.swapBuffers();
    }
    else if (moved && !drawing) {
        // Turn Blink() off
        timer.cancel();

        // Reset the pixels that the cursor was on prior to moving
        ResetPrevious();

        backgroundLayer.fillCircle(cursor.x, cursor.y, cursor.r, draw_color);

        backgroundLayer.swapBuffers();

        // Turn Blink() back on
        // Blink();
        timer.every(250, Blink);
    }

    /// Anytime we move, we want to reset all the pixels that the cursor was over
    /// to their previous values if we're not drawing

}
