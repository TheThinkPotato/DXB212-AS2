/*
Interactive Magic Crystal Ball code by Daniel Lopez, LCD code based of the Liquad Crystal Examples.
The firmware has been designed for the tangible media unit DXB212 for Queensland University of Technology.

IR Wave Detection
=================================
 * IR Proximety sensor input set to pin 8

Reed Switches
=================================
  * Gimble Forward pin 16
  * Gimble Backward pin 17
  * Gimble Left pin 18
  * Gimble Right pin 19

Twist Pot Analog Input
=================================
  * Analog Twist Pot pin A0 (D14)

FAN PWM
=================================
 * Fan PWM output on pin 6

LCD SCREEN
=================================
 * LCD RS pin to digital pin 12
 * LCD Enable pin to digital pin 11
 * LCD D4 pin to digital pin 5
 * LCD D5 pin to digital pin 4
 * LCD D6 pin to digital pin 3
 * LCD D7 pin to digital pin 2
 * LCD R/W pin to ground
 * LCD VSS pin to ground
 * LCD VCC pin to 5V
 * 3k ohm resistor to LCD VO pin (pin 3) and ground
*/

// include the library code:
#include <LiquidCrystal.h>
#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <avr/power.h>  // Required for 16 MHz Adafruit Trinket
#include "ReadDebounce.h"


#define PIN_NEO_PIXEL 7     // Arduino pin that connects to NeoPixel
#define NUM_PIXELS 24       // The number of LEDs (pixels) on NeoPixel
#define DELAY_INTERVAL 250  // 250ms pause between each pixel

// functions
void ReedSwitches();
void MotionDetect();
void debugLCD();
void initLCD();
void updateInputs();
void ControlFan();

// Inputs
const uint8_t POT = A0;
const uint8_t IR_PIN = 8;
const uint8_t REED_FORWARD = 16;
const uint8_t REED_BACKWARD = 17;
const uint8_t REED_LEFT = 18;
const uint8_t REED_RIGHT = 19;

// Outputs
const uint8_t FAN = 6;

// Application Specfic
const bool DEV_MODE = 1;
const uint8_t FAN_INC = 25;
const uint8_t DIVIDER_AMOUNT = 8;  // Amount of times to skip before ramping down fan
uint8_t waveCnt = 0;
uint8_t fanVolume = 0;
uint16_t divider = 0;  // For Fan ramp down
uint8_t potValue = 0;
uint8_t pixel = 0;


// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

ReadDebounce motionIR(IR_PIN);
ReadDebounce reedForward(REED_FORWARD);
ReadDebounce reedBackward(REED_BACKWARD);
ReadDebounce reedLeft(REED_LEFT);
ReadDebounce reedRight(REED_RIGHT);

Adafruit_NeoPixel NeoPixel(NUM_PIXELS, PIN_NEO_PIXEL, NEO_GRB + NEO_KHZ800);
//=================================================================
// Setup
void setup() {

  NeoPixel.begin();  // INITIALIZE NeoPixel strip object (REQUIRED)

  //Setup debug LCD if in devMode
  if (DEV_MODE) {
    // set up the LCD's number of columns and rows:
    lcd.begin(16, 2);
    // Print a message to the LCD.
    lcd.print("MagicBall 0.1b");
    lcd.setCursor(0, 1);
    lcd.print("Waves:");
  }

  // Inputs
  // IR Detection Pin
  pinMode(IR_PIN, INPUT);

  // Reed Switches
  pinMode(REED_FORWARD, INPUT_PULLUP);
  pinMode(REED_BACKWARD, INPUT_PULLUP);
  pinMode(REED_LEFT, INPUT_PULLUP);
  pinMode(REED_RIGHT, INPUT_PULLUP);



  // Outputs
  pinMode(FAN, OUTPUT);
}

//=================================================================
// Main Loop
void loop() {

  updateInputs();
  ReedSwitches();
  MotionDetect();
  ControlFan();
  debugLCD();
  NeoPixelRun();
}

//=================================================================
// Debug LCD
void debugLCD() {
  // Display + for rising edge for reed switch
  if (reedForward.isRisingEdge()) {

    lcd.setCursor(9, 1);
    lcd.print("+");

    // Display - for falling edge for reed switch
  } else if (reedForward.isFallingEdge()) {

    lcd.setCursor(9, 1);
    lcd.print("-");
  }

  //Rising Edge  __|^^
  // Display rising edge as R
  if (motionIR.isRisingEdge()) {
    lcd.setCursor(15, 0);
    lcd.print("R");

    // Display falling edge as F
  } else if (motionIR.isFallingEdge()) {
    lcd.setCursor(15, 0);
    lcd.print("F");
  }

  // Display amount of waves performed
  lcd.setCursor(6, 1);
  lcd.print(waveCnt);
  lcd.print("  ");

  // Display Fan Speed of PWM
  lcd.setCursor(12, 1);
  lcd.print(fanVolume);
  lcd.print("  ");

  // Display POT
  lcd.setCursor(10, 0);
  lcd.print(potValue);
  lcd.print(" ");
}


//=================================================================
// Update all states for

void updateInputs() {
  // Get ADC of POT
  potValue = analogRead(POT);

  // update reedSwitches
  reedForward.updateState();
  reedBackward.updateState();
  reedLeft.updateState();
  reedRight.updateState();

  // read motion input status
  motionIR.updateState();
}


//=================================================================
// Motion Detection
// Get motion detect and trigger actions
void MotionDetect() {

  //Rising Edge  __|^^
  if (motionIR.isRisingEdge()) {
  }

  //Falling Edge ^^|__
  else if (motionIR.isFallingEdge()) {
    waveCnt++;
    // Increase PWM Duty cycle if not of 100%
    if (fanVolume < (255 - FAN_INC))
      fanVolume += FAN_INC;
  }
}


//=================================================================
// Reed Switches
// Get reed switches and trigger actions
void ReedSwitches() {

  if (reedForward.isRisingEdge()) {

  } else if (reedForward.isFallingEdge()) {
  }

  if (reedBackward.isRisingEdge()) {

  } else if (reedBackward.isFallingEdge()) {
  }

  if (reedLeft.isRisingEdge()) {

  } else if (reedLeft.isFallingEdge()) {
  }

  if (reedRight.isRisingEdge()) {

  } else if (reedRight.isFallingEdge()) {
  }
}


//=================================================================
// Fan speed cotnrol
void ControlFan() {
  // Decay divider to slow the time fan to turn off
  divider++;

  //Fan volume is not off with divider
  if (fanVolume != 0 && divider % DIVIDER_AMOUNT == 0)
    analogWrite(FAN, fanVolume--);
}


//=================================================================
// Neo Pixel
void NeoPixelRun() {
  NeoPixel.clear();  // set all pixel colors to 'off'. It only takes effect if pixels.show() is called

  // turn pixels to green one by one with delay between each pixel
  // for (int pixel = 0; pixel < NUM_PIXELS; pixel++) {           // for each pixel
  
  if (pixel % NUM_PIXELS != 0) {
    NeoPixel.setPixelColor(pixel, NeoPixel.Color(fanVolume/8, fanVolume/8, fanVolume));  // it only takes effect if pixels.show() is called
    NeoPixel.show();                                       // send the updated pixel colors to the NeoPixel hardware.
  } else {
    pixel = 0;
    NeoPixel.setPixelColor(pixel, NeoPixel.Color(fanVolume/8, fanVolume/8, fanVolume));  // it only takes effect if pixels.show() is called
    NeoPixel.show();          
  }
  pixel++;
  // }
}