/*
Date:26/4/2024
Created By Mohamed Iraqi
This file defines Core functions for Communication with The arduino Code side of the code
*/

#include "ArdCom.h"

#include <LiquidCrystal_I2C.h>
//I2C pins declaration
// Initialize the LiquidCrystal_I2C library with the LCD address and dimensions
#define lcdColumns 16
#define lcdRows 2
#define lcdAddress 0x27  // You may need to adjust this address based on your LCD module
// Set the LCD address and dimensions
LiquidCrystal_I2C lcd(lcdAddress, lcdColumns, lcdRows, LCD_5x10DOTS);


#define PowerArraySize 3 //Number of devices

/*Usage Variables*/
double TotalEnergySincePowerUp[PowerArraySize] = { 0 };
double EnergyHourly[24][PowerArraySize] = { 0 };
double EnergyNow[PowerArraySize] = { 0 };

double power[3] = { 0 };

void setup() {
  lcd.begin();      //Defining 16 columns and 2 rows of lcd display
  lcd.backlight();  //To Power ON the back light

  // put your setup code here, to run once:
  ArdCom_Init();
}

int Hold_Millis_SendPower_Var = 0;
void loop() {



    ArdCom_Com_Handler();
  ArdCom_StatusConnectedWifi();
}
/*testing energyhourly
  lcd.print(":1:" + String(EnergyHourly[ArdCom_MyHour()][0]));
  delay(1000);
  lcd.clear();
  lcd.print("2:" + String(EnergyHourly[ArdCom_MyHour()][1]));
  delay(1000);
  lcd.clear();
  lcd.print("3:" + String(EnergyHourly[ArdCom_MyHour()][2]));
  delay(1000);
  lcd.clear();
  lcd.print("4:" + String(TotalEnergySincePowerUp[0]));
  delay(1000);
  lcd.clear();
  lcd.print("5:" + String(TotalEnergySincePowerUp[1]));
  delay(1000);
  lcd.clear();
  lcd.print("6:" + String(TotalEnergySincePowerUp[2]));
  delay(1000);
  
  for (int ii = 0; ii < 24; ii++) {
  lcd.clear();
  delay(100);
  lcd.clear();
    lcd.clear();
  lcd.print(String(ii)+":1.2:" +String(EnergyHourly[ii][1]));
  delay(100);
  lcd.clear();
    lcd.clear();
  lcd.print(String(ii)+":1.3:" +String(EnergyHourly[ii][2]));
  delay(100);
  lcd.clear();
  }
  */