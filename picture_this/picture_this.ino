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
SMARTMATRIX_ALLOCATE_INDEXED_LAYER(indexedLayer, kMatrixWidth, kMatrixHeight, COLOR_DEPTH, kIndexedLayerOptions);

// Blink the cursor every x
auto timer = timer_create_default();


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
const int kPercent = 35;  // 50 = 50%, 25 = 25%, etc.
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
const int kWhitePin  = 24;  // TODO: Select pins
const int kRedPin    = 25;
const int kGreenPin  = 26;
const int kBluePin   = 27;
const int kYellowPin = 28;
const int kOrangePin = 29;
const int kPurplePin = 30;
const int kErasePin  = 31;

// Const values to use for functionality button pins
const int kDrawPin   = 32;  // TODO: Select pins
const int kResetPin  = 33;
const int kCursorPin = 34;

// Const values to use for joystick pins
const int kStickUpPin = 39;  // TODO: Select pins
const int kStickDnPin = 38;
const int kStickLtPin = 40;
const int kStickRtPin = 41;

const int kCursorLayer = 1;

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

void RecordLast() {
    blink = false;
    cursor.pr = cursor.r;
    cursor.py = cursor.y;
    cursor.px = cursor.x;
}

void Blink() {
    // If not drawing, blink cursor
    struct rgb24 blink_color;
    if (blink) {  // Blink is on, turn off
        indexedLayer.setIndexedColor(kCursorLayer, {0x0, 0x0, 0x0});
        indexedLayer.fillScreen(kCursorLayer);
        // blink_color = kClear;
    }
    else {  // Blink is off, turn on
        indexedLayer.setIndexedColor(kCursorLayer, current_color);
        switch (cursor.r) {
            case 1:
            {
                Serial.printf("Cursor size 1. Drawing pixel");
                indexedLayer.drawPixel(cursor.x, cursor.y, kCursorLayer);
                break;
            }
            case 2:
            {
                indexedLayer.drawPixel(cursor.x, cursor.y, kCursorLayer);       // Middle
                indexedLayer.drawPixel(cursor.x - 1, cursor.y, kCursorLayer);   // Left middle
                indexedLayer.drawPixel(cursor.x + 1, cursor.y, kCursorLayer);   // Right middle

                indexedLayer.drawPixel(cursor.x, cursor.y - 1, kCursorLayer);   // Top middle
                indexedLayer.drawPixel(cursor.x - 1, cursor.y - 1, kCursorLayer);   // Top left
                indexedLayer.drawPixel(cursor.x + 1, cursor.y - 1, kCursorLayer);   // Top right

                indexedLayer.drawPixel(cursor.x, cursor.y + 1, kCursorLayer);   // Bottom middle
                indexedLayer.drawPixel(cursor.x - 1, cursor.y + 1, kCursorLayer);   // Bottom left
                indexedLayer.drawPixel(cursor.x + 1, cursor.y + 1, kCursorLayer);   // Bottom right
                break;
            }
            case 3:
            {
                break;
            }
            default:
                break;
        }
        // blink_color = current_color;
    }

    indexedLayer.swapBuffers();
    // backgroundLayer.fillCircle(
    //         cursor.x, cursor.y, cursor.r, blink_color);
    // backgroundLayer.swapBuffers();
    blink = !blink;
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

    // Initialize I2C communication and Qwiic relay object
    if (!relay.begin()) {
        Debug("Couldn't establish I2C connection with Qwiic relay");
    } else {
        Debug("Established I2C connection with Qwiic relay");
        relay.turnRelayOn(kTopMatrix);
        relay.turnRelayOn(kBottomMatrix);
    }

    matrix.addLayer(&backgroundLayer);
    matrix.addLayer(&indexedLayer);
    matrix.begin();
    matrix.setBrightness(kBrightness);
    backgroundLayer.enableColorCorrection(true);

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
        Serial.printf("Changing to purple");
        current_color = kPurple;
    }
    else if (btn_erase.update() && btn_erase.fallingEdge()) {
        current_color = kClear;
    }

    if (btn_draw.update() && btn_draw.fallingEdge()) {
        drawing = !drawing;
        Blink();
    }
    
    if (btn_reset.update() && btn_reset.fallingEdge()) {
        backgroundLayer.fillScreen(kClear);
        backgroundLayer.swapBuffers();

        drawing = false;

        RecordLast();

        cursor.r = 1;
        cursor.x = kCenterX;
        cursor.y = kCenterY;

        Debug("Resetting board");
    }

    if (btn_cursor.update() && btn_cursor.fallingEdge()) {
        cursor.pr = cursor.r;   // Record last cursor size for blinking
        cursor.r = (cursor.r % 3) + 1;  // Allow radii 1,2,3

        // Erase previous cursor
        blink = true;
        Blink(); // Call Blink to erase previous cursor

        Serial.printf("[DEBUG]: New cursor size: %d\n", cursor.r);
    }



    /**************************************/
    /*           Joystick Polling         */
    /**************************************/
    bool moved = false;
    if (stick_up.update() && stick_up.fallingEdge()) {
        Debug("Joystick up");
        RecordLast();
        cursor.y += 1;
        moved = true;
        if (cursor.y > 63) cursor.y = 0;  // wrap around
    }

    else if (stick_dn.update() && stick_dn.fallingEdge()) {
        Debug("Joystick down");
        RecordLast();
        cursor.y -= 1;
        moved = true;
        if (cursor.y < 0) cursor.y = 63;  // wrap around
    }
    
    else if (stick_lt.update() && stick_lt.fallingEdge()) {
        Debug("Joystick left");
        RecordLast();
        cursor.x -= 1;
        moved = true;
        if (cursor.x < 0) cursor.x = 63;  // wrap around
    }
    
    else if (stick_rt.update() && stick_rt.fallingEdge()) {
        Debug("Joystick right");
        RecordLast();
        cursor.x += 1;
        moved = true;
        if (cursor.x > 63) cursor.x = 0;  // wrap around
    }


    /**************************************/
    /*               Drawing              */
    /**************************************/
    if (moved && drawing) {
        backgroundLayer.fillCircle(
                    cursor.px, cursor.py, cursor.pr, current_color);
        backgroundLayer.swapBuffers();
        Debug("Drawing");
    }
    else if (moved && !drawing) {
        backgroundLayer.fillCircle(
            cursor.px, cursor.py, cursor.pr, kClear);
            backgroundLayer.swapBuffers();
        timer.cancel();
        Blink();
        timer.every(250, Blink);
    }
}
