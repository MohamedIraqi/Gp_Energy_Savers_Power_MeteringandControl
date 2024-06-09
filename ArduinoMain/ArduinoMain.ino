/*
Date:26/4/2024
Created By Mohamed Iraqi
This is the main code for Gp Project.
This Code Reads from Current and Voltage Sensors --> then displays readings on lcd and sends Data to esp to upload on Cloud Database.
*/

#include "ReadSensors.h"
#include "time.h"

#define PowerArraySize 3

#define EspRstPin 13 //D13

/*Usage Variables*/
double TotalEnergySincePowerUp[PowerArraySize] = { 0 };
double EnergyHourly[24][3] = { 0 };
double EnergyNow[PowerArraySize] = { 0 };

/*Sensor Variables*/
float CtArray[PowerArraySize] = { 0, 0, 0 };
float PtArray[] = { 0 };
double power[PowerArraySize] = { 0, 0, 0 };

/*Timing Variables*/
int BeforeMeasure_Time = 0;
float Delta_Time = 0;
int HourNow_Time = 0;
int DayNow_Time = 0;

int SendPowerDelayms = 4999;
int Hold_Min_SendPower_VarBefre = 3;

int EspRstInt = 0;



void setup() {
  pinMode(EspRstPin, INPUT);
  //digitalWrite(EspRstPin, 0);

  Serial.begin(115200);
  ReadSensors_Init();
  ReadSensors_GetInitHour();
  ReadSensors_GetInitDay();
  delay(600);

}

void loop() {
  //check if we are connected with esp
  while ((String) "YesConnected" != ReadSensors_SendRequest(IsConnected_Enum)) {
    if (EspRstInt >= 3) {
  pinMode(EspRstPin, OUTPUT);

      digitalWrite(EspRstPin, 1);
      delay(1000);
  pinMode(EspRstPin, INPUT);

      //digitalWrite(EspRstPin, 0);
      Serial.println("***********+______________+**********************");
      delay(3000);
      EspRstInt=0;
    }
      Serial.println(EspRstInt);

    delay(100);
    EspRstInt++;
  }

  Main_CalculatePower();

  Main_SendPower();

  // //Show Time (Testing purposes)
  // Test_ShowTime();

  Main_CalculateEnergyArrays();

  Main_ResetArrays();
  ReadSensors_SendRequest(BreakOut_Enum);
}
