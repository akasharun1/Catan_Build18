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
#define RingArduinoCode greenController

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
#define STEPSIZE 8

#define END_TURN 8

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

bool endTurnButtonState = false;
bool prevEndTurnButtonState = false;
bool endTurn = false;

bool diceRollButtonState = false;
bool prevDiceRollButtonState = false;
bool readDice = false;

bool leverButtonState = false;
bool prevLeverButtonState = false;
bool readLever = false;

void setup() {
  Serial.begin(9600);
  // while(1) {
  //   while (Serial.available() > 0) {
  //     Serial.println(Serial.read());
  //     //Serial.println("receiving");
  //   }
  //   //Serial.println("not receiving");
  // }
  
 //Serial.println("Basic Encoder Test:");


  //pinMode(rxPin, INPUT);
  //pinMode(txPin, OUTPUT);
  pinMode(SWITCH_UP, INPUT_PULLUP);
  pinMode(SWITCH_DOWN, INPUT_PULLUP);
  pinMode(END_TURN, INPUT_PULLUP);

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
  //Serial.println("Setup finished");

}

void loop() {
  size_t newPosition = myEnc.read() + 300;
  if (newPosition != oldPosition) {
    oldPosition = newPosition;
  }

  lc.setDigit(0, LEDSIZE - 1, woodCount, (newPosition/STEPSIZE) % 5 == 0);
  lc.setDigit(0, LEDSIZE - 2, clayCount, (newPosition/STEPSIZE) % 5 == 1);
  lc.setDigit(0, LEDSIZE - 3, sheepCount, (newPosition/STEPSIZE) % 5 == 2);
  lc.setDigit(0, LEDSIZE - 4, wheatCount, (newPosition/STEPSIZE) % 5 == 3);
  lc.setDigit(0, LEDSIZE - 5, rockCount, (newPosition/STEPSIZE) % 5 == 4);
  
  // Serial.print((int)rockCount);
  // Serial.print(" ");
  // Serial.print((int)wheatCount);
  // Serial.print(" ");
  // Serial.print((int)sheepCount);
  // Serial.print(" ");
  // Serial.print((int)clayCount);
  // Serial.print(" ");
  // Serial.println((int)woodCount);

  int messageLengthCount = 0;

  char buf[64];
  int iter = 0;
  int currRead = 0;

  if (Serial.available() <= 0) {
    currRead = 'e';
  } else {
    currRead = Serial.read();
  }  //Serial.println("receiving");
  
  
  // Message parsing and sending
  while ((currRead != 'e')) {
    buf[iter] = currRead;
    iter++;
    while (Serial.available() <= 0);
    currRead = Serial.read();  
  }
  buf[iter] = 'e';
  iter++;

  if (iter != 1) {
    buf[iter] = '\0';

    for (int i = 0; i < iter; i++) {
      if (buf[i] == RingArduinoCode && iter >= i + 6) {
        woodCount = (woodCount + buf[i+1]) % 10;
        sheepCount = (sheepCount + buf[i+2]) % 10;
        clayCount = (clayCount + buf[i+3]) % 10;
        wheatCount = (wheatCount + buf[i+4]) % 10;
        rockCount = (rockCount + buf[i+5]) % 10;
        buf[i+6] = endTurn;
        buf[i+7] = readDice;
        endTurn = false;
        readDice = false;
        break;
      }
    }
    //Serial.println(buf);
    for (int i = 0; i < iter; i++) {
      Serial.write(buf[i]);
    }
  }
  

  bool leverUp = digitalRead(SWITCH_UP) == LOW;
  bool leverDown = digitalRead(SWITCH_DOWN) == LOW;
  leverButtonState = leverUp || leverDown;
  if (leverButtonState != prevLeverButtonState) {
    if (leverButtonState == true) {
        readLever = true;
    }
  }
  prevLeverButtonState = leverButtonState;

  // code for Switch
  if (leverUp && readLever) {
    switch ((newPosition/STEPSIZE) % 5) {
      case (0): {
        woodCount++;
        break;
      }
      case (1): {
        clayCount++;
         break;
      }
      case (2): {
        sheepCount++;
         break;
      }
      case (3): {
        wheatCount++;
         break;
      }
      case (4): {
        rockCount++;
         break;
      }
    }
    readLever = false;
  }
  else if (leverDown && readLever) {
    switch ((newPosition/STEPSIZE) % 5) {
      case (0): {
         woodCount = woodCount == 0 ? 0 : woodCount - 1;
         break;
      }
      case (1): {
        clayCount = clayCount == 0 ? 0 : clayCount - 1;
         break;
      }
      case (2): {
        sheepCount = sheepCount == 0 ? 0 : sheepCount - 1;
         break;
      }
      case (3): {
        wheatCount = wheatCount == 0 ? 0 : wheatCount - 1;
         break;
      }
      case (4): {
        rockCount = rockCount == 0 ? 0 : rockCount - 1;
        break;
      }
    }
    readLever = false;
  }

  endTurnButtonState = digitalRead(END_TURN);
  // if (endTurnButtonState == LOW) {
  //   delay(40);
  //   endTurnButtonState = digitalRead(END_TURN);
  // }
  if (endTurnButtonState != prevEndTurnButtonState) {
    if (endTurnButtonState == LOW) {
        endTurn = true;
        //Serial.println("End turn");
    }
  }
  prevEndTurnButtonState = endTurnButtonState;
  
  diceRollButtonState = digitalRead(ENC_SW);
  if (diceRollButtonState != prevDiceRollButtonState) {
    if (diceRollButtonState == false) {
        readDice = true;
        //Serial.println("read dice");
    }
  }
  prevDiceRollButtonState = diceRollButtonState;
}
