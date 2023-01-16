#include <SoftwareSerial.h>
#include <LedControl.h>
#include <Arduino.h>
#include "Encoder.h"


// Serial
#define rxPin 10
#define txPin 11

// 7-segment Control
#define DataIn 5
#define Clk 7
#define CS 6
#define ChainLength 1
#define LEDSIZE 5

// Resources
#define ROCK 1
#define CLAY 2
#define WHEAT 3
#define WOOD 4
#define SHEEP 5


// Encoder Specifics
#define ENC_CLK 10
#define ENC_DT 11
#define ENC_SW 8

#define STEPSIZE 4

// Ring Arduino Specific
char RingArduinoCode;

// Change these two numbers to the pins connected to your encoder.
//   Best Performance: both pins have interrupt capability
//   Good Performance: only the first pin has interrupt capability
//   Low Performance:  neither pin has interrupt capability

Encoder myEnc(ENC_CLK,ENC_DT);
//   avoid using pins with LEDs attached



// Set up a new SoftwareSerial object
SoftwareSerial mySerial =  SoftwareSerial(rxPin, txPin);

// Define pins for LedControl. These can be any digital pins on microcontroller.
LedControl lc = LedControl(DataIn, Clk, CS, ChainLength);

char woodCount;
char sheepCount;
char clayCount;
char wheatCount;
char rockCount;

char encoderSwitchState;
char encoderRotaryState;
char sevenSegVal;
size_t oldPosition  = -999;

void setup() {
  Serial.begin(9600);
  Serial.println("Basic Encoder Test:");


  pinMode(rxPin, INPUT);
  pinMode(txPin, OUTPUT);
  mySerial.begin(9600);
  
  pinMode(ENC_SW, INPUT);

  woodCount = 0;
  sheepCount = 0;
  clayCount = 0;
  wheatCount = 0;
  rockCount = 0;

    // Setup for 7-segments

    // Initialize all displays
  for (int dsp = 0; dsp < lc.getDeviceCount(); dsp++) {
    lc.shutdown(dsp, false); // Wakeup display
    lc.setIntensity(dsp, 2); // Set brightness to a lower level (1-15)
    lc.clearDisplay(dsp);   // Clear the display
  }

  RingArduinoCode = 5;

  //
  // if (mySerial.available() > 0) {
  //   RingArduinoCode = mySerial.read();
  //   mySerial.write(RingArduinoCode + 1);
  //}

}

//long oldPosition  = -999;

void loop() {
  
  size_t newPosition = myEnc.read();
  if (newPosition != oldPosition) {
    oldPosition = newPosition;
    Serial.println(newPosition);
  }


  

  lc.setDigit(0, LEDSIZE - 1, 5, (newPosition/STEPSIZE) % 5 == 0);
  lc.setDigit(0, LEDSIZE - 2, sheepCount, (newPosition/STEPSIZE) % 5 == 1);
  lc.setDigit(0, LEDSIZE - 3, clayCount, (newPosition/STEPSIZE) % 5 == 2);
  lc.setDigit(0, LEDSIZE - 4, wheatCount, (newPosition/STEPSIZE) % 5 == 3);
  lc.setDigit(0, LEDSIZE - 5, rockCount, (newPosition/STEPSIZE) % 5 == 4);

  


    int messageLengthCount = 0;

    // Message parsing and sending
    if (mySerial.available() > 0) {
        if (mySerial.read() == 's') {

            // Stream starts with an s and ends with an e
            mySerial.write('s');

            char currRead;
            while ((currRead = mySerial.read()) != 'e') {

                // Each Arduino has a seperate Code after whose 7 bytes
                // are data values relevant to it

                if (currRead == RingArduinoCode) {
                    mySerial.write(RingArduinoCode);

                    // Should we use this as a way to reset the rotary state? not hard
                    mySerial.read();
                    mySerial.write(encoderRotaryState);
                    
                    woodCount += mySerial.read(); 
                    mySerial.write(woodCount);

                    sheepCount += mySerial.read();
                    mySerial.write(sheepCount);

                    clayCount += mySerial.read();
                    mySerial.write(clayCount);

                    wheatCount += mySerial.read();
                    mySerial.write(wheatCount);

                    rockCount += mySerial.read();
                    mySerial.write(rockCount);
                }
                else {
                    mySerial.write(currRead);
                }
            }

            mySerial.write('e');
        }
    }
 
}


/*

#include <SoftwareSerial.h>
#include <LedControl.h>
#include <Arduino.h>
#include "Encoder.h"


// Serial
#define rxPin 10
#define txPin 11

// 7-segment Control
#define DataIn 5
#define Clk 7
#define CS 6
#define ChainLength 1
#define LEDSIZE 5

#define ROCK 1
#define CLAY 2
#define WHEAT 3
#define WOOD 4
#define SHEEP 5

#define DOWN 0
#define UP 1
#define switchPin 8

#define MAXPOS 19
#define MINPOS 0
#define STEPSIZE 4

// Ring Arduino Specific
char RingArduinoCode;

// Change these two numbers to the pins connected to your encoder.
//   Best Performance: both pins have interrupt capability
//   Good Performance: only the first pin has interrupt capability
//   Low Performance:  neither pin has interrupt capability
Encoder myEnc(5, 6);
//   avoid using pins with LEDs attached

// Set up a new SoftwareSerial object
SoftwareSerial mySerial =  SoftwareSerial(rxPin, txPin);

// Define pins for LedControl. These can be any digital pins on microcontroller.
LedControl lc = LedControl(DataIn, Clk, CS, ChainLength);

char woodCount;
char sheepCount;
char clayCount;
char wheatCount;
char rockCount;

char encoderSwitchState;
char encoderRotaryState;
char sevenSegVal;
long oldPosition  = -999;

void setup()  {

    // Setup for Software Serial
    Serial.begin(9600);
    Serial.println("Boasic Encoder Test:");
    /*
    pinMode(rxPin, INPUT);
    pinMode(txPin, OUTPUT);
    mySerial.begin(9600);

    Serial.begin(9600);

    pinMode(switchPin, INPUT);

    woodCount = 0;
    sheepCount = 0;
    clayCount = 0;
    wheatCount = 0;
    rockCount = 0;

    // Setup for 7-segments

    // Initialize all displays
    for (int dsp = 0; dsp < lc.getDeviceCount(); dsp++) {
        lc.shutdown(dsp, false); // Wakeup display
        lc.setIntensity(dsp, 2); // Set brightness to a lower level (1-15)
        lc.clearDisplay(dsp);   // Clear the display
    }


    Serial.println("Basic Encoder Test:");

    


   

	// Dynamically set the unique arduino code
	//RingArduinoCode = mySerial.read();
	//mySerial.write(RingArduinoCode + 1);
}

void loop() {

  long newPosition = myEnc.read();
  if (newPosition != oldPosition) {
    oldPosition = newPosition;
    Serial.println(newPosition);
  }
    
  /*
    lc.setDigit(0, LEDSIZE - 1, woodCount, newPosition/STEPSIZE == 0);
    lc.setDigit(0, LEDSIZE - 2, sheepCount, newPosition/STEPSIZE == 1);
    lc.setDigit(0, LEDSIZE - 3, clayCount, newPosition/STEPSIZE == 2);
    lc.setDigit(0, LEDSIZE - 4, wheatCount, newPosition/STEPSIZE == 3);
    lc.setDigit(0, LEDSIZE - 5, rockCount, newPosition/STEPSIZE == 4);

    int messageLengthCount = 0;

    // Message parsing and sending
    if (mySerial.available() > 0) {
        if (mySerial.read() == 's') {

            // Stream starts with an s and ends with an e
            mySerial.write('s');

            char currRead;
            while ((currRead = mySerial.read()) != 'e') {

                // Each Arduino has a seperate Code after whose 7 bytes
                // are data values relevant to it

                if (currRead == RingArduinoCode) {
                    mySerial.write(RingArduinoCode);

                    // Should we use this as a way to reset the rotary state? not hard
                    mySerial.read();
                    mySerial.write(encoderRotaryState);
                    
                    woodCount += mySerial.read(); 
                    mySerial.write(woodCount);

                    sheepCount += mySerial.read();
                    mySerial.write(sheepCount);

                    clayCount += mySerial.read();
                    mySerial.write(clayCount);

                    wheatCount += mySerial.read();
                    mySerial.write(wheatCount);

                    rockCount += mySerial.read();
                    mySerial.write(rockCount);
                }
                else {
                    mySerial.write(currRead);
                }
            }

            mySerial.write('e');
        }
    }
}

*/


// https://lastminuteengineers.com/rotary-encoder-arduino-tutorial/
// http://www.mathertel.de/Arduino/RotaryEncoderLibrary.aspx
