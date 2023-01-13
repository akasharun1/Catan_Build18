#include <SoftwareSerial.h>
#include <LedControl.h>
#include <Arduino.h>
#include <RotaryEncoder.h>

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

#define DOWN 0
#define UP 1
#define switchPin 8

#define MAXPOS 19
#define MINPOS 0
#define STEPSIZE 4

// Ring Arduino Specific
char RingArduinoCode;

#if defined(ARDUINO_AVR_UNO) || defined(ARDUINO_AVR_NANO_EVERY)
// Example for Arduino UNO with input signals on pin 2 and 3
#define PIN_IN1 A2
#define PIN_IN2 A3

#elif defined(ESP8266)
// Example for ESP8266 NodeMCU with input signals on pin D5 and D6
#define PIN_IN1 D5
#define PIN_IN2 D6

#endif

// A pointer to the dynamic created rotary encoder instance.
// This will be done in setup()
RotaryEncoder *encoder = nullptr;

#if defined(ARDUINO_AVR_UNO) || defined(ARDUINO_AVR_NANO_EVERY)
// This interrupt routine will be called on any change of one of the input signals
void checkPosition()
{
  encoder->tick(); // just call tick() to check the state.
}

#elif defined(ESP8266)
/**
 * @brief The interrupt service routine will be called on any change of one of the input signals.
 */
IRAM_ATTR void checkPosition()
{
  encoder->tick(); // just call tick() to check the state.
}

#endif


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
int pos;


void setup()  {

    // Setup for Software Serial
    pinMode(rxPin, INPUT);
    pinMode(txPin, OUTPUT);
    mySerial.begin(9600);

    pinMode(switchPin, INPUT);

    woodCount = 0;
    sheepCount = 0;
    clayCount = 0;
    wheatCount = 0;
    rockCount = 0;

    pos = 0;

    // Setup for 7-segments

    // Initialize all displays
    for (int dsp = 0; dsp < lc.getDeviceCount(); dsp++) {
        lc.setScanLimit(dsp, LEDSIZE); // Limit scan to size of LED module
        lc.shutdown(dsp, false); // Wakeup display
        lc.setIntensity(dsp, 2); // Set brightness to a lower level (1-15)
        lc.clearDisplay(dsp);   // Clear the display
    }


    // setup the rotary encoder functionality

    // use FOUR3 mode when PIN_IN1, PIN_IN2 signals are always HIGH in latch position.
    // encoder = new RotaryEncoder(PIN_IN1, PIN_IN2, RotaryEncoder::LatchMode::FOUR3);

    // use FOUR0 mode when PIN_IN1, PIN_IN2 signals are always LOW in latch position.
    // encoder = new RotaryEncoder(PIN_IN1, PIN_IN2, RotaryEncoder::LatchMode::FOUR0);

    // use TWO03 mode when PIN_IN1, PIN_IN2 signals are both LOW or HIGH in latch position.
    encoder = new RotaryEncoder(PIN_IN1, PIN_IN2, RotaryEncoder::LatchMode::TWO03);

    // register interrupt routine
    attachInterrupt(digitalPinToInterrupt(PIN_IN1), checkPosition, CHANGE);
    attachInterrupt(digitalPinToInterrupt(PIN_IN2), checkPosition, CHANGE);

    encoder->tick();
   // encoderSwitchState = encoder->

	// Dynamically set the unique arduino code
	RingArduinoCode = mySerial.read();
	mySerial.write(RingArduinoCode + 1);
}

void loop() {
    encoder->tick(); // just call tick() to check the state.
    newPos = encoder->getPosition();
	pos = newPos >= MAXPOS ? MAXPOS : (newPos <= MINPOS ? MINPOS : newPos);

    // if(encoder-> != encoderSwitchState) {
    //     encoderSwitchState = !encoderSwitchState;
    //     switch (pos) {
    //         case 0 ... 3: break;
    //         case 4 ... 7: break;
    //         case 8 ... 11: break;
    //         case 12 ... 15: break;
    //         case 16 ... 19: break;
    //         default: 
    //     }
    // }

    lc.setDigit(0, LEDSIZE - 1, woodCount, pos/STEPSIZE == 0);
    lc.setDigit(0, LEDSIZE - 2, sheepCount, pos/STEPSIZE == 1);
    lc.setDigit(0, LEDSIZE - 3, clayCount, pos/STEPSIZE == 2);
    lc.setDigit(0, LEDSIZE - 4, wheatCount, pos/STEPSIZE == 3);
    lc.setDigit(0, LEDSIZE - 5, rockCount, pos/STEPSIZE == 4);

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


// To use other pins with Arduino UNO you can also use the ISR directly.
// Here is some code for A2 and A3 using ATMega168 ff. specific registers.

// Setup flags to activate the ISR PCINT1.
// You may have to modify the next 2 lines if using other pins than A2 and A3
//   PCICR |= (1 << PCIE1);    // This enables Pin Change Interrupt 1 that covers the Analog input pins or Port C.
//   PCMSK1 |= (1 << PCINT10) | (1 << PCINT11);  // This enables the interrupt for pin 2 and 3 of Port C.

// The Interrupt Service Routine for Pin Change Interrupt 1
// This routine will only be called on any signal change on A2 and A3.
// ISR(PCINT1_vect) {
//   encoder->tick(); // just call tick() to check the state.
// }

// The End


// https://lastminuteengineers.com/rotary-encoder-arduino-tutorial/
// http://www.mathertel.de/Arduino/RotaryEncoderLibrary.aspx
