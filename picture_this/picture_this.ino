#include <Wire.h>
#include <MatrixHardware_Teensy4_ShieldV5.h>
#include <SmartMatrix.h>
#include <Bounce.h>
#include "SparkFun_Qwiic_Relay.h"

#include <string>

// COMMENT THIS OUT BEFORE SHIPPING. Controls debug prints
#define DEBUG

/*****************************************************************************/
/*      THIS CODE COPIED FROM SMART MATRIX LIBRARY FOR INITIALIZATION        */
/*****************************************************************************/
#define COLOR_DEPTH 24                  // known working: 24, 48 - If the sketch uses type `rgb24` directly, COLOR_DEPTH must be 24
const uint16_t kMatrixWidth = 32;        // known working: 32, 64, 96, 128
const uint16_t kMatrixHeight = 32;       // known working: 16, 32, 48, 64
const uint8_t kRefreshDepth = 36;       // known working: 24, 36, 48
const uint8_t kDmaBufferRows = 4;       // known working: 2-4, use 2 to save memory, more to keep from dropping frames and automatically lowering refresh rate
const uint8_t kPanelType = SMARTMATRIX_HUB75_32ROW_MOD16SCAN;   // use SMARTMATRIX_HUB75_16ROW_MOD8SCAN for common 16x32 panels
const uint32_t kMatrixOptions = (SMARTMATRIX_OPTIONS_NONE);      // see http://docs.pixelmatix.com/SmartMatrix for options
const uint8_t kBackgroundLayerOptions = (SM_BACKGROUND_OPTIONS_NONE);
const uint8_t kScrollingLayerOptions = (SM_SCROLLING_OPTIONS_NONE);
const uint8_t kIndexedLayerOptions = (SM_INDEXED_OPTIONS_NONE);

SMARTMATRIX_ALLOCATE_BUFFERS(matrix, kMatrixWidth, kMatrixHeight, kRefreshDepth, kDmaBufferRows, kPanelType, kMatrixOptions);
SMARTMATRIX_ALLOCATE_BACKGROUND_LAYER(backgroundLayer, kMatrixWidth, kMatrixHeight, COLOR_DEPTH, kBackgroundLayerOptions);
SMARTMATRIX_ALLOCATE_SCROLLING_LAYER(scrollingLayer, kMatrixWidth, kMatrixHeight, COLOR_DEPTH, kScrollingLayerOptions);
SMARTMATRIX_ALLOCATE_INDEXED_LAYER(indexedLayer, kMatrixWidth, kMatrixHeight, COLOR_DEPTH, kIndexedLayerOptions);


/*****************************************************************************/
/*                      CUSTOM GLOBALS FOR PICTURE THIS                      */
/*****************************************************************************/
// Holds the color being drawn. Initialized to no value / black / clear
struct rgb24 current_color = {0x00, 0x00, 0x00};

// Used to update current_color
const struct rgb24 kWhite = {0xff, 0xff, 0xff};
const struct rgb24 kRed = {0xff, 0x00, 0x00};
const struct rgb24 kGreen = {0x00, 0xff, 0x00};
const struct rgb24 kBlue = {0x00, 0x00, 0xff};
const struct rgb24 kYellow = {0xff, 0xff, 0x00};
const struct rgb24 kOrange = {0xff, 0xA5, 0x00};
const struct rgb24 kPurple = {0x80, 0x00, 0x80};
const struct rgb24 kClear = {0x00, 0x00, 0x00};

// Holds the cursor size
// TODO: Figure out how to store cursor size

// Screen brightness. Change kPercent to change brightness
const int kPercent = 50;  // 50 = 50%, 25 = 25%, etc.
const int kBrightness = (kPercent * 255) / 100;  // <-- Don't change this

// Const values to use for milliseconds argument to Bounce() object constructor
const int kBounceBtn   = 50;  // For buttons
const int kBounceStick = 50;  // For joystick

// Const values to use for color button pins
const int kWhitePin  = 0;  // TODO: Select pins
const int kRedPin    = 0;
const int kGreenPin  = 0;
const int kBluePin   = 0;
const int kYellowPin = 0;
const int kOrangePin = 0;
const int kPurplePin = 0;
const int kErasePin  = 0;

// Const values to use for functionality button pins
const int kDrawPin   = 0;  // TODO: Select pins
const int kResetPin  = 0;
const int kCursorPin = 0;

// Const values to use for joystick pins
const int kStickUpPin = 0;  // TODO: Select pins
const int kStickDnPin = 0;
const int kStickLtPin = 0;
const int kStickRtPin = 0;

// Bounce objects for color buttons
Bounce btn_white  = Bounce(kWhitePin,  kBounceBtn);
Bounce btn_red    = Bounce(kRedPin,    kBounceBtn);
Bounce btn_green  = Bounce(kGreenPin,  kBounceBtn);
Bounce btn_blue   = Bounce(kBluePin,   kBounceBtn);
Bounce btn_yellow = Bounce(kYellowPin, kBounceBtn);
Bounce btn_orange = Bounce(kOrangePin, kBounceBtn);
Bounce btn_purple = Bounce(kPurplePin, kBounceBtn);
Bounce btn_erase  = Bounce(kErasePin,  kBounceBtn);

// TODO: Select pins to use
// Bounce objects for module functionality
Bounce btn_draw   = Bounce(kDrawPin,   kBounceBtn);
Bounce btn_reset  = Bounce(kResetPin,  kBounceBtn);
Bounce btn_cursor = Bounce(kCursorPin, kBounceBtn);

// Bounce objects for the joystick
Bounce stick_up = Bounce(kStickUpPin, kBounceStick);
Bounce stick_dn = Bounce(kStickDnPin, kBounceStick);
Bounce stick_lt = Bounce(kStickLtPin, kBounceStick);
Bounce stick_rt = Bounce(kStickRtPin, kBounceStick);

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
    Serial.printf(("\033[31m[DEBUG]:\033[0m " + msg).c_str());
#endif
}


/*****************************************************************************/
/*                                  SETUP                                    */
/*****************************************************************************/
void setup()
{
    // Start the serial monitor first. Don't do anything till it's up.
    Serial.begin(57600);
    while(!Serial);

    // Set Bounce object pins to INPUT_PULLUP, as suggested by the library
    pinMode(kWhitePin,  INPUT_PULLUP);
    pinMode(kRedPin,    INPUT_PULLUP);
    pinMode(kGreenPin,  INPUT_PULLUP);
    pinMode(kBluePin,   INPUT_PULLUP);
    pinMode(kYellowPin, INPUT_PULLUP);
    pinMode(kOrangePin, INPUT_PULLUP);
    pinMode(kPurplePin, INPUT_PULLUP);
    pinMode(kErasePin,  INPUT_PULLUP);

    // Initialize I2C communication and Qwiic relay object
    Wire.begin();
    if (!relay.begin()) {
        Debug("Couldn't establish I2C connection with Qwiic relay");
    } else {
        Debug("Established I2C connection with Qwiic relay");
        relay.turnRelayOn(kTopMatrix);
        relay.turnRelayOn(kBottomMatrix);
    }

    matrix.addLayer(&backgroundLayer); 
    matrix.addLayer(&scrollingLayer); 
    matrix.addLayer(&indexedLayer); 
    matrix.begin();
}



/*****************************************************************************/
/*                                  LOOP                                     */
/*****************************************************************************/
void loop() {}
