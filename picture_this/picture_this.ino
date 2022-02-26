#include <Wire.h>
#include <MatrixHardware_Teensy4_ShieldV5.h>
#include <SmartMatrix.h>
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

SMARTMATRIX_ALLOCATE_BUFFERS(matrix, kMatrixWidth, kMatrixHeight, kRefreshDepth, kDmaBufferRows, kPanelType, kMatrixOptions);
SMARTMATRIX_ALLOCATE_BACKGROUND_LAYER(backgroundLayer, kMatrixWidth, kMatrixHeight, COLOR_DEPTH, kBackgroundLayerOptions);
SMARTMATRIX_ALLOCATE_SCROLLING_LAYER(scrollingLayer, kMatrixWidth, kMatrixHeight, COLOR_DEPTH, kScrollingLayerOptions);
SMARTMATRIX_ALLOCATE_INDEXED_LAYER(indexedLayer, kMatrixWidth, kMatrixHeight, COLOR_DEPTH, kIndexedLayerOptions);


/*****************************************************************************/
/*                      CUSTOM GLOBALS FOR PICTURE THIS                      */
/*****************************************************************************/
// Holds the color being drawn. Initialized to no value / black / clear
struct rgb24 current_color = {0x00, 0x00, 0x00};

// Holds the cursor size
// TODO: Figure out how to store cursor size

// The I2C address of the Qwiic relay
const int kRelayAddr = 0x6D;

// The relay used to supply power to the top matrix
const int kTopMatrix = 1;

// The relay used to supply power to the bottom matrix
const int kBottomMatrix = 2;

// Used to command the Qwiic relay via I2C
Qwiic_Relay relay(kRelayAddr);



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

/// \brief Set current_color to white
/// \remarks Triggered by an interrupt. Must be non-reentrant
void SetColorWhite() {
    current_color.red = 0xff;
    current_color.green = 0xff;
    current_color.blue = 0xff;
}

/// \brief Set current_color to red
/// \remarks Triggered by an interrupt. Must be non-reentrant
void SetColorRed() {
    current_color.red = 0xff;
    current_color.green = 0x00;
    current_color.blue = 0x00;
}

/// \brief Set current_color to green
/// \remarks Triggered by an interrupt. Must be non-reentrant
void SetColorGreen() {
    current_color.red = 0x00;
    current_color.green = 0xff;
    current_color.blue = 0x00;
}

/// \brief Set current_color to blue
/// \remarks Triggered by an interrupt. Must be non-reentrant
void SetColorBlue() {
    current_color.red = 0x00;
    current_color.green = 0x00;
    current_color.blue = 0xff;
}

/// \brief Set current_color to purple
/// \remarks Triggered by an interrupt. Must be non-reentrant
void SetColorPurple() {
    current_color.red = 0x80;
    current_color.green = 0x00;
    current_color.blue = 0x80;
}

/// \brief Set current_color to yellow
/// \remarks Triggered by an interrupt. Must be non-reentrant
void SetColorYellow() {
    current_color.red = 0xff;
    current_color.green = 0xff;
    current_color.blue = 0x00;
}

/// \brief Set current_color to orange
/// \remarks Triggered by an interrupt. Must be non-reentrant
void SetColorOrange() {
    current_color.red = 0xff;
    current_color.green = 0xA5;
    current_color.blue = 0x00;
}

/// \brief Set current_color to clear
/// \remarks Triggered by an interrupt. Must be non-reentrant
void SetColorClear() {
    current_color.red = 0x00;
    current_color.green = 0x00;
    current_color.blue = 0x00;
}


/*****************************************************************************/
/*                                  SETUP                                    */
/*****************************************************************************/
void setup()
{
    // Start the serial monitor first. Don't do anything till it's up.
    Serial.begin(57600);
    while(!Serial);

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
