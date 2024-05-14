/*
Date:26/4/2024
Created By Mohamed Iraqi
This file defines Core functions for Communication with The arduino Code side of the code
*/

#include "ArdCom.h"

float power[] = { 0, 0, 0 };

void setup() {
  // put your setup code here, to run once:
  ArdCom_Init();
}

void loop() {
  // put your main code here, to run repeatedly:
  ArdCom_Com_Handler();
  ArdCom_StatusConnectedWifi();
}
