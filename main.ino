/*
IR Wave Detection
=================================
 * IR set to pin 8

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

const uint8_t IR_PIN = 8;
uint8_t waveCnt = 0;
uint8_t irState = 0;
// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

void setup()
{
  // IR Detection Pin
  pinMode(IR_PIN, INPUT);
  
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  // Print a message to the LCD.
  lcd.print("FANCY LAMP TEST 0.1b");
  lcd.setCursor(0, 1);        
  lcd.print("Waves:");
  
}

void loop()
{
  // set the cursor to column 0, line 1
  // (note: line 1 is the second row, since counting begins with 0):
  getIRState();
  
  //Rising Edge
  if(irState == 0b00111111)
  {    
    waveCnt++;
    lcd.setCursor(6, 1);        
    lcd.print(waveCnt);
    lcd.setCursor(15, 0);     
    lcd.print("R");
  }
  //Falling Edge
  else if(irState == 0b11111100 )
  {
    lcd.setCursor(15, 0);     
    lcd.print("F");
  }

  // lcd.setCursor(0, 1);
  // print the number of seconds since reset:
  // lcd.print(millis() / 1000);
}

void getIRState()
{    
  uint8_t temp = digitalRead(IR_PIN);  
  
  irState = irState << 1;
  irState |= temp ;
  
  lcd.setCursor(10,1);
  lcd.print(temp);
  lcd.setCursor(12,1);
  lcd.print(irState);
  lcd.print("  ");
  return temp;
}
