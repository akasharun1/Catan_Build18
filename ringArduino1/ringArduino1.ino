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

// Switch Specifics
#define SWITCH_UP 10
#define SWITCH_DOWN 11

#define STEPSIZE 4

// Change these two numbers to the pins connected to your encoder.
//   Best Performance: both pins have interrupt capability
//   Good Performance: only the first pin has interrupt capability
//   Low Performance:  neither pin has interrupt capability

Encoder myEnc(ENC_CLK,ENC_DT);
//   avoid using pins with LEDs attached

// Define pins for LedControl. These can be any digital pins on microcontroller.
LedControl lc = LedControl(DataIn, Clk, CS, ChainLength);


char endTurnSwitchState;
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
  endTurnSwitchState = 0x0;
 //Serial.println("Basic Encoder Test:");


  //pinMode(rxPin, INPUT);
  //pinMode(txPin, OUTPUT);
  pinMode(SWITCH_UP, INPUT_PULLUP);
  pinMode(SWITCH_DOWN, INPUT_PULLUP);

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
    //Serial.println(newPosition);
  }

  lc.setDigit(0, LEDSIZE - 1, woodCount, (newPosition/STEPSIZE) % 5 == 0);
  lc.setDigit(0, LEDSIZE - 2, sheepCount, (newPosition/STEPSIZE) % 5 == 1);
  lc.setDigit(0, LEDSIZE - 3, clayCount, (newPosition/STEPSIZE) % 5 == 2);
  lc.setDigit(0, LEDSIZE - 4, wheatCount, (newPosition/STEPSIZE) % 5 == 3);
  lc.setDigit(0, LEDSIZE - 5, rockCount, (newPosition/STEPSIZE) % 5 == 4);

  int messageLengthCount = 0;

//  while(Serial.available() > 0) {
//    Serial.write(Serial.read());
//  }
  char buf[64];
  int iter = 0;
  int currRead = 0;

  if (Serial.available() <= 0) {
    currRead = 'e';
  } else {
    currRead = Serial.read();
  }
  
  // Message parsing and sending
  while ((currRead != 'e')) {
    buf[iter] = currRead;
    iter++;
    while (Serial.available() <= 0);
    currRead = Serial.read();
    //Serial.println(currRead);
    
  }
  buf[iter] = 'e';
  iter++;

  //Serial.println("Done Reading");

  if (iter != 1) {
    buf[iter] = '\0';
    //Serial.write(buf);

    for (int i = 0; i < iter; i++) {
      if (buf[i] == RingArduinoCode && iter >= i + 6) {
        woodCount = (woodCount + buf[i+1]) % 10;
        sheepCount = (sheepCount + buf[i+2]) % 10;
        clayCount = (clayCount + buf[i+3]) % 10;
        wheatCount = (wheatCount + buf[i+4]) % 10;
        rockCount = (rockCount + buf[i+5]) % 10;
        buf[i+6] = endTurnSwitchState;
        break;
      }
    }
  }

  // code for Switch
  if (digitalRead(SWITCH_UP) == LOW) {
    Serial.println("UP");
  }
  else if (digitalRead(SWITCH_DOWN) == LOW) {
    Serial.println("DOWN");
  }
  else {
    Serial.println("NONE");
  }

}
