#include <SoftwareSerial.h>
#include "LedControl.h"

// Serial
#define rxPin 10
#define txPin 11

// 7-segment Control
#define DataIn 5
#define Clk 7
#define CS 6
#define ChainLength 1
#define LEDSIZE 5

#define SHEEP 0
#define WOOD 1
#define CLAY 2
#define WHEAT 3
#define IRON 4

#define OFF 0
#define ON 1

// Ring Arduino Specific
#define RingArduinoCode 0x61


// Set up a new SoftwareSerial object
SoftwareSerial mySerial =  SoftwareSerial(rxPin, txPin);

// Define pins for LedControl. These can be any digital pins on microcontroller.
LedControl lc = LedControl(DataIn, Clk, CS, ChainLength);

char woodCount;
char sheepCount;
char clayCount;
char wheatCount;
char ironCount;

char encoderSwitchState;
char encoderRotaryState;
char sevenSegVal;


void setup()  {

    // Setup for Software Serial
    pinMode(rxPin, INPUT);
    pinMode(txPin, OUTPUT);
    mySerial.begin(9600);


    woodCount = 0;
    sheepCount = 0;
    clayCount = 0;
    wheatCount = 0;
    ironCount = 0;

    encoderRotaryState = SHEEP;
    encoderSwitchState = OFF;

    // Setup for 7-segments

    // Initialize all displays
    for (int dsp = 0; dsp < lc.getDeviceCount(); dsp++) {
        lc.setScanLimit(dsp, LEDSIZE); // Limit scan to size of LED module
        lc.shutdown(dsp, false); // Wakeup display
        lc.setIntensity(dsp, 2); // Set brightness to a lower level (1-15)
        lc.clearDisplay(dsp);   // Clear the display
    }



}

void loop() {

    int messageLengthCount = 0;

    // Message parsing and sending

    if (mySerial.available() > 0) {
        if (mySerial.read() == 's') {

            // Stream starts with an s and ends with an e
            mySerial.write('s');

            while (mySerial.read() != 'e') {

                // Each Arduino has a seperate Code after whose 7 bytes
                // are data values relevant to it

                if (mySerial.read() == RingArduinoCode) {
                    mySerial.write(RingArduinoCode);

                    mySerial.read();
                    mySerial.write(encoderSwitchState);
                    mySerial.read();
                    mySerial.write(encoderRotaryState);

                    woodCount = mySerial.read();
                    mySerial.write(woodCount);

                    sheepCount = mySerial.read();
                    mySerial.write(sheepCount);

                    clayCount = mySerial.read();
                    mySerial.write(clayCount);

                    wheatCount = mySerial.read();
                    mySerial.write(wheatCount);

                    ironCount = mySerial.read();
                    mySerial.write(ironCount);
                }
                else {
                    mySerial.write(mySerial.read());
                }
            }

            mySerial.write('e');
        }
    }
    lc.setDigit(0, LEDSIZE - 1, woodCount, false);
    lc.setDigit(0, LEDSIZE - 2, sheepCount, false);
    lc.setDigit(0, LEDSIZE - 3, clayCount, false);
    lc.setDigit(0, LEDSIZE - 4, wheatCount, false);
    lc.setDigit(0, LEDSIZE - 4, ironCount, false);


}


// https://lastminuteengineers.com/rotary-encoder-arduino-tutorial/
// http://www.mathertel.de/Arduino/RotaryEncoderLibrary.aspx
