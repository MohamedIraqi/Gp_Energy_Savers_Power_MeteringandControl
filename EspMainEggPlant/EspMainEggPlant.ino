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

void setup() {

  ArdCom_Init();
}

int Hold_Millis_SendPower_Var = 0;
void loop() {
  ArdCom_lcd_DataDisp();
  ArdCom_Com_Handler();

  //test
  // Populate arrays with random numbers
  for (size_t i = 0; i < PowerArraySize; ++i) {
    TotalEnergySincePowerUp[i] = static_cast<double>(std::rand()) / RAND_MAX * 1010;  // Random number between 0 and 1000
    EnergyNow[i] = static_cast<double>(std::rand()) / RAND_MAX * 1010;                // Random number between 0 and 1000
    power[i] = static_cast<double>(std::rand()) / RAND_MAX * 1010;
    for (size_t j = 0; j < 31; ++j) {
      EnergyHourly[j][i] = static_cast<double>(std::rand()) / RAND_MAX * 1010;  // Random number between 0 and 1000
    }

  }

  ArdCom_UploadData();
}
