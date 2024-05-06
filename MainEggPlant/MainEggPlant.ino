/*
Date:26/4/2024
Created By Mohamed Iraqi
This is the main code for Gp Project.
This Code Reads from Current and Voltage Sensors --> then displays readings on lcd and sends Data to esp to upload on Cloud Database.
*/

#include "ReadSensors.h"
#include "LowPower.h"

#define PowerArraySize 3

float CtArray[] = { 0, 0, 0 };
float PtArray[] = { 0 };
float power[] = { 0, 0, 0 };
void setup() {
  Serial.begin(115200);
  ReadSensors_Init();
}

void loop() {
  ReadSensors_Measure(CtArray, PtArray);
  ReadSensors_LCDDisplayMeasurements(CtArray, PtArray);
  for (int i = 0; i < PowerArraySize; i++) {
    power[i] = CtArray[i] * PtArray[0];
  }
  ReadSensors_SendPowerReadToEsp(power);
  ReadSensors_LcdDisplay(ReadSensors_SendRequest(hour_Enum) + ":" + ReadSensors_SendRequest(minute_Enum) + ":" + ReadSensors_SendRequest(second_Enum));
  // Enter power down state for 8 s with ADC and BOD module disabled
  LowPower.powerDown(SLEEP_2S, ADC_ON, BOD_ON);
}
