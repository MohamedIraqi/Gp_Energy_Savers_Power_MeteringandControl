/*
Date:26/4/2024
Created By Mohamed Iraqi
@author Mohamed Iraqi - M.A.Iraqi.Amen@gmail.com
This file defines Core functions for Communication with The arduino Code side of the code
*/

#include "ArdCom.h"

#define PowerArraySize 3           //Number of devices
#define EnergyHourlyArraySize 24   //Number of hours

/*Usage Variables*/
float TotalEnergySincePowerUp[PowerArraySize] = { 0 };
float EnergyHourly[EnergyHourlyArraySize][PowerArraySize] = { 0 };
float EnergyNow[PowerArraySize] = { 0 };

float power[3] = { 0 };

int MonthNow_Time;
int DayNow_Time;
int HourNow_Time;

bool breakout;

void setup() {

  ArdCom_Init();
}

int Hold_Millis_SendPower_Var = 0;
void loop() {
  breakout = false;
  while(!breakout) {
  //ArdCom_lcd_DataDisp();
  ArdCom_Com_Handler();
  }



  ArdCom_UploadData();
}
