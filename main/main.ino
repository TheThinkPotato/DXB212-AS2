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

// Inputs
#define POT A0           // Base Potentiometer
#define IR_PIN 8         // IR Wave Detection Pin
#define REED_FORWARD 16  // Push Forwards
#define REED_BACKWARD 17
#define REED_LEFT 18
#define REED_RIGHT 19

// OutPuts
#define FAN 6            // PWM Fan
#define PIN_NEO_PIXEL 7  // Arduino pin that connects to NeoPixel

// Neo Pixel
#define NUM_PIXELS 24  // The number of LEDs (pixels) on NeoPixel

// Appplication Specific
#define DEV_MODE 1        // Dev Mode On / Off
#define DIVIDER_AMOUNT 8  // Amount of times to skip before ramping down fan
#define FAN_INC 25        // Fan Incement amount on wave

// functions
void ReedSwitches();
void MotionDetect();
void debugLCD();
void initLCD();
void updateInputs();
void ControlFan();


// Application Specfic
uint8_t neoColorScene = 0;  // 0: Purple, 1: Pink , 2: Blue , 3: Red
uint8_t fadeCounter = 0;
uint8_t fadeSpeed = 2;
bool fadeDirUp = 1;

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
    neoColorScene = 0;
  } else if (reedForward.isFallingEdge()) {
  }

  if (reedBackward.isRisingEdge()) {
    neoColorScene = 1;
  } else if (reedBackward.isFallingEdge()) {
  }

  if (reedLeft.isRisingEdge()) {
    neoColorScene = 2;
  } else if (reedLeft.isFallingEdge()) {
  }

  if (reedRight.isRisingEdge()) {
    neoColorScene = 3;
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

  // Set pulse speed.
  if (fanVolume >= 80) {
    fadeSpeed = 1;
  } else {
    fadeSpeed = 2;
  }

  // Set direction of fase
  if (fadeCounter >= 150)
    fadeDirUp = 0;
  else if (fadeCounter <= 30)
    fadeDirUp = 1;

  //Set pulse rate of fade
  if (fadeDirUp && divider % fadeSpeed == 0) {
    fadeCounter++;
    if (fanVolume > 140)
      fadeCounter++;
  } else if (divider % fadeSpeed == 0) {
    fadeCounter--;
    if (fanVolume > 140)
      fadeCounter--;
  }


  NeoPixel.clear();
  float red = 0;
  float green = 0;
  float blue = 0;

  //Scene Selector
  switch (neoColorScene) {
    case 0:
      //Purple
      red = 0.667;
      green = 0;
      blue = 1;
      break;

    case 1:
      //Pink
      red = 1;
      green = 0;
      blue = 0.667;
      break;


    case 2:
      // Blue
      red = 0;
      green = 0.334;
      blue = 0.667;
      break;

    case 3:
      //Red
      red = 1;
      green = 0.02;
      blue = 0.01;
      break;
  }

  for (int pixels = 0; pixels < NUM_PIXELS; pixels++) {

    int lightLevelR = fadeCounter * red;
    int lightLevelG = fadeCounter * green;
    int lightLevelB = fadeCounter * blue;

    // Random White Sparkle when fan is fast

    if (fanVolume >= 180 && fanVolume < 225) {
      if (random(1, 40) == 1) {
        lightLevelR = 255;
        lightLevelG = 255;
        lightLevelB = 255;
      }
    } else if (fanVolume >= 225) {
      if (random(1, 10) == 1) {
        lightLevelR = 255;
        lightLevelG = 255;
        lightLevelB = 255;
      }
    }

    NeoPixel.setPixelColor(pixels, NeoPixel.Color(lightLevelR, lightLevelG, lightLevelB));  // it only takes effect if pixels.show() is called
  }
  NeoPixel.show();



  // switch (fadeCounter) {
  //   case 0:
  //     neo0per();
  //     break;

  //   case 45:
  //     neo25per();
  //     break;

  //   case 125:
  //     neo50per();
  //     break;

  //   case 180:
  //     neo75per();
  //     break;

  //   case 250:
  //     neo100per();
  //     break;
}



// if (pixel % NUM_PIXELS != 0) {
//   NeoPixel.setPixelColor(pixel, NeoPixel.Color(fanVolume / 8, fanVolume / 8, fanVolume));  // it only takes effect if pixels.show() is called
//   NeoPixel.show();                                                                         // send the updated pixel colors to the NeoPixel hardware.
// } else {
//   pixel = 0;
//   NeoPixel.setPixelColor(pixel, NeoPixel.Color(fanVolume / 8, fanVolume / 8, fanVolume));  // it only takes effect if pixels.show() is called
//   NeoPixel.show();
// }
// pixel++;
// }
// }

void neo0per() {
  NeoPixel.clear();
  NeoPixel.show();
}


void neo25per() {
  NeoPixel.clear();
  for (int pixels = 0; pixels < NUM_PIXELS; pixels++) {
    NeoPixel.setPixelColor(pixels, NeoPixel.Color(150 / 4, 0, 200 / 4));  // it only takes effect if pixels.show() is called
  }
  NeoPixel.show();
}

void neo50per() {
  NeoPixel.clear();
  for (int pixels = 0; pixels < NUM_PIXELS; pixels++) {
    NeoPixel.setPixelColor(pixels, NeoPixel.Color(150 / 3, 0, 200 / 3));  // it only takes effect if pixels.show() is called
  }
  NeoPixel.show();
}


void neo75per() {
  NeoPixel.clear();
  for (int pixels = 0; pixels < NUM_PIXELS; pixels++) {
    NeoPixel.setPixelColor(pixels, NeoPixel.Color(150 / 2, 0, 200 / 2));  // it only takes effect if pixels.show() is called
  }
  NeoPixel.show();
}


void neo100per() {
  NeoPixel.clear();
  for (int pixels = 0; pixels < NUM_PIXELS; pixels++) {
    NeoPixel.setPixelColor(pixels, NeoPixel.Color(150 / 1, 0, 200 / 1));  // it only takes effect if pixels.show() is called
  }
  NeoPixel.show();
}