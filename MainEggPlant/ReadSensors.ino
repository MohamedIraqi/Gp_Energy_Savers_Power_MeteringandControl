/*
Date:26/4/2024
Created By Mohamed Iraqi
This file implements Core functions for hardware side of the code
*/

//Define Comm Phrases
#define Message_Ended "DataReceived"
#define Terminator_Char "$"

#include <ZMPT101B.h>

//#include "ReadSensors.h"
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define SENSITIVITY 269.0f
ZMPT101B voltageSensor(A0, 50.0);

float getVPP(int sensorTA12);

//I2C pins declaration
// Initialize the LiquidCrystal_I2C library with the LCD address and dimensions
#define lcdColumns 16
#define lcdRows 2
#define lcdAddress 0x27  // You may need to adjust this address based on your LCD module
// Set the LCD address and dimensions
LiquidCrystal_I2C lcd(lcdAddress, lcdColumns, lcdRows, LCD_5x10DOTS);


// Define analog input pins
const int CTPin1 = A0;
const int CTPin2 = A1;
const int CTPin3 = A2;
const int PTPin1 = A3;

int CTPins[] = { CTPin1, CTPin2, CTPin3 };
int PTPins[] = { PTPin1 };

float CTVPP;          // Voltage measured across resistor
float PTVPP;          // Voltage measured across resistor
float CTthisReading;  //Actual RMS current in Wire for this instant
float PTthisReading;  //Actual RMS current in Wire for this instant



/**
     * Init for the lcd and required inputs for measurment
     *
     * @param . void.
     * @return void
     *
     */
void ReadSensors_Init() {
  lcd.begin();      //Defining 16 columns and 2 rows of lcd display
  lcd.backlight();  //To Power ON the back light
  voltageSensor.setSensitivity(SENSITIVITY);
  delay(100);

  for (int CtLoopI = 0; CtLoopI < 3; CtLoopI++) {
    pinMode(CTPins[CtLoopI], INPUT);
  }
  for (int PtLoopI = 0; PtLoopI < 1; PtLoopI++) {
    pinMode(PTPins[PtLoopI], INPUT);
  }

  lcd.print("Analog Readings");
  delay(1000);
  lcd.clear();
}

/**
     * Measure Current and Voltage Value
     *
     * @param CtArray The Array of current sensors (Returns readings of cts)
     * @param CtArraySize The size of the Array of current sensors 
     * @param PtArray The Array of Voltage sensors (Returns readings of Pts)
     * @param PtArraySize The size of the Array of Voltage sensors
     * @param NumOfCyclesToAverage The size of the Array of Voltage sensors
     * @return void
     *
     **/
void ReadSensors_Measure(float* CtArray, float* PtArray, int NumOfCyclesToAverage = 1, int CtArraySize = 3, int PtArraySize = 1) {
  for (int CtLoopI = 0; CtLoopI < CtArraySize; CtLoopI++) { CtArray[CtLoopI] = 0; }  //all variables=0
  for (int PtLoopI = 0; PtLoopI < PtArraySize; PtLoopI++) { PtArray[PtLoopI] = 0; }

  for (int AvgCycles = 0; AvgCycles < NumOfCyclesToAverage; AvgCycles++) {

    for (int CtLoopI = 0; CtLoopI < CtArraySize; CtLoopI++) {
      float CTVPP = getVPP(CTPins[CtLoopI]);

      CTthisReading = (CTVPP / 200.0) * 1000.0 * 0.707 * 1000;  // Actual RMS current in Wire /200 for the 200 resisotr *1000 for 1000:1 ratio *0.707 for rms *1000 to get mA
      CtArray[CtLoopI] += CTthisReading / NumOfCyclesToAverage;
    }
    for (int PtLoopI = 0; PtLoopI < PtArraySize; PtLoopI++) {
      float PTVPP = getVPP(PTPins[PtLoopI]);

      PTthisReading = voltageSensor.getRmsVoltage();

      //https://electropeak.com/learn/interfacing-zmpt101b-voltage-sensor-with-arduino/

      PtArray[PtLoopI] += PTthisReading / NumOfCyclesToAverage;
    }
  }
  return;
}

/**
     * Display Ct and Pt values on lcd
     *
     * @param CtArray The Array of current sensors (Returns readings of cts)
     * @param CtArraySize The size of the Array of current sensors 
     * @param PtArray The Array of Voltage sensors (Returns readings of Pts)
     * @param PtArraySize The size of the Array of Voltage sensors
     * @return void
     *
     **/
void ReadSensors_LCDDisplayMeasurements(float* CtArray, float* PtArray, int CtArraySize = 3, int PtArraySize = 1) {
  //lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("PT:");
  lcd.print(PtArray[0], 0);  // Display PTvoltage with 0 decimal places (3+3+1=7)
  lcd.print("V     ");
  lcd.setCursor(8, 0);
  lcd.print("CT1:");
  lcd.print(CtArray[0], 0);  // Display CTReading with 0 decimal places (4+3+2=11)
  lcd.print("mA    ");

  lcd.setCursor(0, 1);
  lcd.print("CT2:");
  lcd.print(CtArray[1], 0);  // Display CTReading with 0 decimal places (4+3+2=8)
  lcd.print("mA    ");
  lcd.setCursor(9, 1);
  lcd.print("CT3:");
  lcd.print(CtArray[2], 0);  // Display CTReading with 0 decimal places (4+3+2=16)
  lcd.print("mA    ");
}

/**
     * Send Power Reading to Esp
     *
     * @param power The power Reading
     * @return True if transaction completed, false if transaction failed
     *
     **/
bool ReadSensors_SendPowerReadToEsp(float* power, int size = 3) {
  bool Transaction = false;
  String ReceivedDataStringBUFFER;
  String SendingString;

  if (Serial.available() < 0) {
    for (int i = 0; (i < 50 && (Serial.available() < 0)); i++) {
      delay(5);
    }
  } else if (Serial.available() > 0) {
    ReceivedDataStringBUFFER = Serial.readStringUntil('$');
    if (ReceivedDataStringBUFFER == "SendData") {
      for (int i = 0; i < size; i++) {
        Serial.print("Power");
        Serial.print(String(i));
        Serial.print("$");
        Serial.flush();
        SendingString = String(power[i]);
        Serial.print(SendingString + '$');
        Serial.flush();
        delay(5);
      }
      Serial.print("DataReceived$");
      Serial.flush();
      Serial.print("DataReceived$");
      Serial.flush();
      delay(5);

      Transaction = true;
    }
  }

  return Transaction;
}

/**
     * Send Request to Esp
     *
     * @param Request The power array Readings
     * @return The requested String
     *
     **/
String ReadSensors_SendRequest(CommEnum_t Request) {
  String ReceivedDataStringBUFFER = "";
  Serial.print(Request);
  Serial.print("$");
  Serial.print(Message_Ended);
  Serial.print("$");
  Serial.flush();
  if (Serial.available() == 0) {
    for (int i = 0; (i < 100 && (Serial.available() == 0)); i++) {
      delay(5);
    }
  };
  ReceivedDataStringBUFFER = Serial.readStringUntil('$');
  return ReceivedDataStringBUFFER;
}

/**
     * Send Request to Esp
     *
     * @param ToDisp The String to be displayed
     * @return null
     *
     **/
void ReadSensors_LcdDisplay(String toDisp, bool clear = true, int cursorx = 0, int cursory = 0) {
  if (clear) {
    lcd.clear();
  }
  lcd.setCursor(cursorx, cursory);
  lcd.print(toDisp);
}

/**
     * Get Initial Hour correctly
     *
     * @param null
     * @return null
     *
     **/
void ReadSensors_GetInitHour() {
  //Get HourNow make sure it's correct
  for (int i = 0; (i < 300); i++) {
    HourNow_Time = ReadSensors_SendRequest(hour_Enum).toInt();
    delay(500);
    if ((HourNow_Time == ReadSensors_SendRequest(hour_Enum).toInt() && HourNow_Time >= 0 && HourNow_Time < 24)) {
      break;
    }
  }
}

/**
     * checks if Esp is Online(connected to wifi)
     *
     * @param null
     * @return ture if online or false if offline
     *
     **/
bool ReadSensors_EspOnlineStatus() {
  bool status = true;
  if (Serial.available() == 0) {
    for (int i = 0; (i < 50 && (Serial.available() == 0)); i++) {
      delay(5);
    }
  };
  if ((String)Serial.read() == ".") {
    status = false;
  }
  return status;
}

float getVPP(int sensorTA12) {
  float result;
  int readValue;     //value read from the sensor
  int maxValue = 0;  // store max value here
  int minValue = 0;  // store min value here
  uint32_t start_time = millis();
  while ((millis() - start_time) < 20)  //sample for 1 Sec
  {
    readValue = analogRead(sensorTA12);
    // see if you have a new maxValue
    if (readValue > maxValue) {
      /*record the maximum sensor value*/
      maxValue = readValue;
    }
    if (readValue < minValue) {
      /*record the maximum sensor value*/
      minValue = readValue;
    }
  }
  //https://kitsguru.myshopify.com/products/5a-range-single-phase-ac-current-sensor-module/
  // Convert the digital data to a voltage
  result = ((maxValue - minValue) * 5) / 1024.0;  //2.56

  return result;
}