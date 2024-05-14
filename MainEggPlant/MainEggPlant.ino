/*
Date:26/4/2024
Created By Mohamed Iraqi
This is the main code for Gp Project.
This Code Reads from Current and Voltage Sensors --> then displays readings on lcd and sends Data to esp to upload on Cloud Database.
*/

#include "ReadSensors.h"
#include "LowPower.h"
#include "time.h"

#define PowerArraySize 3

/*Usage Variables*/
double TotalEnergySincePowerUp[PowerArraySize] = { 0 };
double EnergyHourly[24][3] = { 0 };
float EnergyNow[PowerArraySize] = { 0 };

/*Sensor Variables*/
float CtArray[PowerArraySize] = { 0, 0, 0 };
float PtArray[] = { 0 };
float power[PowerArraySize] = { 0, 0, 0 };

/*Timing Variables*/
int BeforeMeasure_Time = 0;
float Delta_Time = 0;
int HourNow_Time = 0;

void setup() {
  Serial.begin(115200);
  ReadSensors_Init();
  ReadSensors_GetInitHour();
}

void loop() {
  BeforeMeasure_Time = millis();
  ReadSensors_Measure(CtArray, PtArray, 10);
  ReadSensors_LCDDisplayMeasurements(CtArray, PtArray);
  for (int i = 0; i < PowerArraySize; i++) {
    power[i] = CtArray[i] * PtArray[0];
  }

 ReadSensors_SendPowerReadToEsp(power);


  /*Energy Calculations*/

  //Append Total energy
  for (int i = 0; i < PowerArraySize; i++) {
    TotalEnergySincePowerUp[i] += EnergyNow[i];
  }

  //Append Energy Hourly
  if (ReadSensors_EspOnlineStatus()) {
    int HourOld_Time = HourNow_Time;
    HourNow_Time = ReadSensors_SendRequest(hour_Enum).toInt();
    if (!(HourNow_Time == HourOld_Time || HourNow_Time == (HourOld_Time + 1)))  //check correct time
    {
      HourNow_Time = HourOld_Time;
    }
    for (int i = 0; i < PowerArraySize; i++) {
      EnergyHourly[HourNow_Time][i] += EnergyNow[i];
    }
  }

  //Calculate Energy
  Delta_Time = (float)(millis() - BeforeMeasure_Time) / 3600000;  //Convert ms to hour /1000 /60 /60
  for (int i = 0; i < PowerArraySize; i++) {

    if (Delta_Time <= 0) { break; }  //Edge case covered

    EnergyNow[i] = power[i] * Delta_Time;
  }
  

}

/* Test Low power mode (failed due to millis stoping)
  Enter power down state for 8 s with ADC and BOD module disabled
  LowPower.powerDown(SLEEP_8S, ADC_ON, BOD_ON);
  */

/*Display Test time
ReadSensors_LcdDisplay(ReadSensors_SendRequest(hour_Enum) + ":" + ReadSensors_SendRequest(minute_Enum) + ":" + ReadSensors_SendRequest(second_Enum));
*/

/*Display test Total array and H array
  for (int i = 0; i < PowerArraySize; i++) {
    ReadSensors_LcdDisplay(String(i) + ",T:" + String(TotalEnergySincePowerUp[i]));
    delay(1000);
  }
  for (int i = 0; i < PowerArraySize; i++) {
    ReadSensors_LcdDisplay(String(i) + ",H["+String(HourNow_Time)+"]:" + String(EnergyHourly[HourNow_Time][i]));
    delay(1000);
  }
*/
