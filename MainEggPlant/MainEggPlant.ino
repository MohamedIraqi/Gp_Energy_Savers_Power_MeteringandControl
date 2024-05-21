/*
Date:26/4/2024
Created By Mohamed Iraqi
This is the main code for Gp Project.
This Code Reads from Current and Voltage Sensors --> then displays readings on lcd and sends Data to esp to upload on Cloud Database.
*/

#include "ReadSensors.h"
#include "time.h"

#define PowerArraySize 3

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

int SendPowerDelayms = 4999;
int Hold_Min_SendPower_VarBefre = 3;

void setup() {
  Serial.begin(115200);
  ReadSensors_Init();
  ReadSensors_GetInitHour();
}

void loop() {
  
  //Power Calculations
  ReadSensors_Measure(CtArray, PtArray, 10);
  BeforeMeasure_Time = millis();
  ReadSensors_LCDDisplayMeasurements(CtArray, PtArray);
  for (int i = 0; i < PowerArraySize; i++) {
    power[i] = CtArray[i] * PtArray[0];
  }


  /*Send Power*/
  if ((millis() - Hold_Min_SendPower_VarBefre) >= SendPowerDelayms) {
    ReadSensors_SendPowerReadToEsp(Power_Enum, power);
    ReadSensors_SendPowerReadToEsp(EnergyNow_Enum, EnergyNow);
    ReadSensors_SendPowerReadToEsp(TotalEnergySincePowerUp_Enum, TotalEnergySincePowerUp);
    ReadSensors_SendPowerReadToEsp(EnergyHourly_Enum, power, EnergyHourly);
    Hold_Min_SendPower_VarBefre = millis();
    delay(2000);
  }
  ReadSensors_LcdDisplay(ReadSensors_SendRequest(hour_Enum) + ":" + ReadSensors_SendRequest(minute_Enum) + ":" + ReadSensors_SendRequest(second_Enum));
  delay(5000);

  /*Energy Calculations*/

  //Append Total energy
  for (int i = 0; i < PowerArraySize; i++) {
    TotalEnergySincePowerUp[i] += EnergyNow[i];
  }

  //Append Energy Hourly
  int HourOld_Time = HourNow_Time;
  HourNow_Time = ReadSensors_SendRequest(hour_Enum).toInt();
  if (!(HourNow_Time == HourOld_Time || HourNow_Time == (HourOld_Time + 1)))  //check correct time
  {
    HourNow_Time = HourOld_Time;
  }
  for (int i = 0; i < PowerArraySize; i++) {
    EnergyHourly[HourNow_Time][i] += EnergyNow[i];
  }


  //Calculate Energy
  Delta_Time = (float)(millis() - BeforeMeasure_Time) / 3600000;  //Convert ms to hour /1000 /60 /60
  for (int i = 0; i < PowerArraySize; i++) {

    if (Delta_Time <= 0) { break; }  //Edge case covered

    EnergyNow[i] = power[i] * Delta_Time;
  }
}

/* Test Low power mode (failed due to millis stoping)
#include "LowPower.h"

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

/*Logging Data on eeprom once every 24Hours or a reset*/
/*if (((ReadSensors_SendRequest(minute_Enum).toInt() - eeprom_read_word(HourHolderNameVarHereLogging)) >= 3) && !DidLogToday) {
    ReadSensors_SaveEEPROM();
    DidLogToday = true;
    eeprom_update_word(&HourHolderNameVarHereLogging, ReadSensors_SendRequest(day_Enum).toInt());
  }
*/