#include <SoftwareSerial.h>
#include <LedControl.h>
#include <Arduino.h>
#include "Encoder.h"

enum ControllerColors {
  blueController = 60,
  redController = 61,
  greenController = 62,
  orangeController = 63
};

// Ring Arduino Specific
#define RingArduinoCode blueController


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
#define ENC_CLK 2
#define ENC_DT 3
#define ENC_SW 4

#define STEPSIZE 4



// Change these two numbers to the pins connected to your encoder.
//   Best Performance: both pins have interrupt capability
//   Good Performance: only the first pin has interrupt capability
//   Low Performance:  neither pin has interrupt capability

Encoder myEnc(ENC_CLK,ENC_DT);
//   avoid using pins with LEDs attached

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


}

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

  while(Serial.available() > 0) {
    Serial.write(Serial.read());
  }

  // Message parsing and sending
//  if (Serial.available() > 0) {
//      if (Serial.read() == 's') {

//          // Stream starts with an s and ends with an e
//          Serial.write('s');

        
//          while (Serial.available() <= 0) ;
//          char currRead = Serial.read();
         
//          while (currRead != 'e' && (Serial.available() > 4)) {
               
//              // Each Arduino has a seperate Code after whose 7 bytes
//              // are data values relevant to it

//              if (currRead == RingArduinoCode) {
//                  Serial.write(currRead);
//                  currRead = Serial.read();
//                  //woodCount += Serial.read();
//                  //Serial.print(woodCount);
//                  //sheepCount += Serial.read();
//                  //Serial.print(sheepCount);
//                  //clayCount += Serial.read();
//                  //Serial.print(clayCount);
//                  //wheatCount += Serial.read();
//                  //Serial.print(wheatCount);
//                  //rockCount += Serial.read();
//                  //Serial.print(rockCount);
//              }
//              else {
//                  Serial.write(currRead);
//                  currRead = Serial.read();
//              }
             
//          }

//          Serial.write('e');
//      }
//  }

}
