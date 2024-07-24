#include <LiquidCrystal_I2C.h>
int count = 0;
LiquidCrystal_I2C lcd(0x27,16,2);  // set the LCD address to 0x3F for a 16 chars and 2 line display

void setup() {
  lcd.init();
  lcd.clear();         
  lcd.backlight();
}

void loop() {
  // Print a message on both lines of the LCD.
  lcd.setCursor(0,0);   //Set cursor to character 2 on line 0
  lcd.print("Hello world!");
  
  lcd.setCursor(0,1);   //Move cursor to character 2 on line 1
  lcd.print(count);
  count = count + 1;
  delay(500);
}
