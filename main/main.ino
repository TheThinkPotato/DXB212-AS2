/*
Interactive SNOW Globe code by Daniel Lopez, LCD code based of the Liquad Crystal Examples.
The firmware has been designed for the tangible media unit DXB212 for Queensland University of Technology.

IR Wave Detection
=================================
 * IR Proximety sensor input set to pin 8

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
 * 10K resistor:
 * ends to +5V and ground
 * wiper to LCD VO pin (pin 3)

*/

// include the library code:
#include <LiquidCrystal.h>
#include <Arduino.h>

const uint8_t FAN_INC= 25;
const uint8_t IR_PIN = 8;
const uint8_t FAN = 6;
uint8_t waveCnt = 0;
uint8_t irState = 0;
uint8_t fanVolume = 0;
uint16_t divider = 0;


// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

void getIRState();



void setup()
{
  // IR Detection Pin
  pinMode(FAN,OUTPUT);
  pinMode(IR_PIN, INPUT);

  
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  // Print a message to the LCD.
  lcd.print("SNOWGLOBE 0.1a");
  lcd.setCursor(0, 1);        
  lcd.print("Waves:");
}

void loop()
{
  // set the cursor to column 0, line 1
  // (note: line 1 is the second row, since counting begins with 0):
  getIRState();
  
  //Rising Edge  __|^^
  if(irState == 0b00111111)
  {    
    lcd.setCursor(15, 0);     
    lcd.print("R");        
  }
  
  //Falling Edge ^^|__
  else if(irState == 0b11111100 )
    {
    lcd.setCursor(15, 0);     
    lcd.print("F");
    waveCnt++;

    // Increase PWM Duty cycle if not of 100%
    if(fanVolume < ( 255 - FAN_INC))
      fanVolume += FAN_INC;

    lcd.setCursor(6, 1);        
    lcd.print(waveCnt);
    lcd.print("  ");
  }

// Decay divider to slow the time fan to turn off
divider++;

//Fan volume is not off with divider
if(fanVolume != 0 && divider % 8 == 0)
 analogWrite(FAN, fanVolume--);
}


// get IR state of the proximimety sensor.
void getIRState()
{    
  uint8_t temp = digitalRead(IR_PIN);  
  
  // debounce pattern (rotate bit to the left and place in new read at LSB)
  irState = irState << 1;
  irState |= temp ;
  
  lcd.setCursor(12,1);
  lcd.print(fanVolume);
  lcd.print("  ");
  return temp;
}
