/*
Date:26/4/2024
Created By Mohamed Iraqi
This file implements Core functions for hardware side of the code
*/

//Define Comm Phrases
#define Message_Ended "DataReceived"
#define Terminator_Char "$"
#define initializor_Char "@"


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
LiquidCrystal_I2C lcd(lcdAddress, lcdColumns, lcdRows);


// Define analog input pins
const int PTPin1 = A0;
const int CTPin1 = A1;
const int CTPin2 = A2;
const int CTPin3 = A3;

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
  voltageSensor.setSensitivity(SENSITIVITY);
  delay(100);

  for (int CtLoopI = 0; CtLoopI < 3; CtLoopI++) {
    pinMode(CTPins[CtLoopI], INPUT);
  }
  for (int PtLoopI = 0; PtLoopI < 1; PtLoopI++) {
    pinMode(PTPins[PtLoopI], INPUT);
  }

  // ReadSensors_LcdDisplay("Analog Readings");
  // delay(1000);
  // lcd.clear();
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
  lcd.begin(lcdColumns, lcdRows, LCD_5x10DOTS);  //Defining 16 columns and 2 rows of lcd display
  lcd.backlight();                               //To Power ON the back light

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
bool ReadSensors_SendPowerReadToEsp(CommEnum_t PowerIdentifier, double* power, double Energyhourlypowertobe[][3] = 0) {
  int Hour_TimeNow_VarF = ReadSensors_SendRequest(hour_Enum).toInt();
  bool Transaction = false;
  switch (PowerIdentifier) {
    case Power_Enum:
    case EnergyNow_Enum:
    case TotalEnergySincePowerUp_Enum:
      {
        Serial.print(initializor_Char);
        Serial.flush();
        Serial.print(PowerIdentifier);
        Serial.print(Terminator_Char);
        for (int i = 0; i < PowerArraySize; i++) {
          delay(5);
          Serial.print(initializor_Char);
          Serial.flush();
          Serial.print(String(power[i]));
          Serial.print(Terminator_Char);
          Serial.flush();
        }
        Serial.print(initializor_Char);
        Serial.flush();
        Serial.print(Message_Ended_Enum);
        Serial.flush();
        Serial.print(Terminator_Char);

        Serial.print(initializor_Char);
        Serial.flush();
        Serial.print(Message_Ended);
        Serial.print(Terminator_Char);
        Serial.flush();
        break;
      }
    case EnergyHourly_Enum:
      {
        Serial.print(initializor_Char);
        Serial.flush();
        Serial.print(PowerIdentifier);
        Serial.print(Terminator_Char);
        Serial.flush();
        //for (int ii = 0; ii < 24; ii++) {
        for (int i = 0; i < PowerArraySize; i++) {
          delay(5);
          Serial.print(initializor_Char);
          Serial.flush();
          Serial.print(String(Energyhourlypowertobe[Hour_TimeNow_VarF][i]));
          Serial.print(Terminator_Char);
          Serial.flush();
        }
        // }
        Serial.print(initializor_Char);
        Serial.flush();
        Serial.print(Message_Ended_Enum);
        Serial.print(Terminator_Char);
        Serial.flush();

        Serial.print(initializor_Char);
        Serial.flush();
        Serial.print(Message_Ended);
        Serial.print(Terminator_Char);
        Serial.flush();
      }
    default:
      break;
  }
  return Transaction;
}

String processSerialString(String receivedString) {
  int index = receivedString.indexOf(initializor_Char);

  if (index >= 0) { // Initializer is found (at the beginning or later)
    return receivedString.substring(index + 1); // Return everything AFTER initializer
  } else { 
    Serial.println("Error: String does not contain initializer character.");
    return "";
  }
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
  Serial.print(initializor_Char);
  Serial.flush();
  Serial.print(Request);
  Serial.print(Terminator_Char);
  Serial.flush();

  Serial.print(initializor_Char);
  Serial.flush();
  Serial.print(Message_Ended_Enum);
  Serial.print(Terminator_Char);
  Serial.flush();

  Serial.print(initializor_Char);
  Serial.flush();
  Serial.print(Message_Ended);
  Serial.print(Terminator_Char);
  Serial.flush();
  if (Serial.available() == 0) {
    for (int i = 0; (i < 100 && (Serial.available() == 0)); i++) {
      delay(5);
    }
  };
  ReceivedDataStringBUFFER = Serial.readStringUntil('$');
  ReceivedDataStringBUFFER = processSerialString(ReceivedDataStringBUFFER);
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
  lcd.begin(lcdColumns, lcdRows, LCD_5x10DOTS);  //Defining 16 columns and 2 rows of lcd display
  lcd.backlight();                               //To Power ON the back light
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
    delay(500);  // delay to insure hour was recieved correctly
    if ((HourNow_Time == ReadSensors_SendRequest(hour_Enum).toInt() && HourNow_Time >= 0 && HourNow_Time < 24)) {
      break;
    }
  }
}

/**
     * Get Initial Day correctly
     *
     * @param null
     * @return null
     *
     **/
void ReadSensors_GetInitDay() {
  //Get DayNow make sure it's correct
  for (int i = 0; (i < 300); i++) {
    DayNow_Time = ReadSensors_SendRequest(day_Enum).toInt();
    delay(500);  // delay to insure Day was recieved correctly
    if ((DayNow_Time == ReadSensors_SendRequest(day_Enum).toInt() && DayNow_Time >= 0 && DayNow_Time < 24)) {
      break;
    }
  }
}

/**
     * CalculatePower
     *
     * @param null
     * @return null
     *
     **/
void Main_CalculatePower() {
  //Power Calculations
  BeforeMeasure_Time = millis();
  ReadSensors_Measure(CtArray, PtArray, 10);
  //ReadSensors_LCDDisplayMeasurements(CtArray, PtArray);
  for (int i = 0; i < PowerArraySize; i++) {
    power[i] = CtArray[i] * PtArray[0];
  }
}

/**
     * Send Power arrays to Esp
     *
     * @param null
     * @return null
     *
     **/
void Main_SendPower() {
  /*Send Power*/
  if ((millis() - Hold_Min_SendPower_VarBefre) >= SendPowerDelayms) {
    Hold_Min_SendPower_VarBefre = millis();  //update hold variable
    ReadSensors_SendPowerReadToEsp(Power_Enum, power);
    ReadSensors_SendPowerReadToEsp(EnergyNow_Enum, EnergyNow);
    ReadSensors_SendPowerReadToEsp(TotalEnergySincePowerUp_Enum, TotalEnergySincePowerUp);
    ReadSensors_SendPowerReadToEsp(EnergyHourly_Enum, power, EnergyHourly);
    delay(2000);
  }
}

/**
     * Calculate Energy arrays 
     *
     * @param null
     * @return null
     *
     **/
void Main_CalculateEnergyArrays() {
  /*Energy Calculations*/

  //Append Total energy
  for (int i = 0; i < PowerArraySize; i++) {
    TotalEnergySincePowerUp[i] += EnergyNow[i];
  }

  //Append Energy Hourly
  int HourOld_Time = HourNow_Time;
  HourNow_Time = ReadSensors_SendRequest(hour_Enum).toInt();
  if (!(HourNow_Time == HourOld_Time || HourNow_Time % 24 == (HourOld_Time + 1) % 24))  //check correct time
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

/**
     * Resets daily arrays after each new day
     *
     * @param null
     * @return null
     *
     **/
void Main_ResetArrays() {
  if (((signed int)ReadSensors_SendRequest(day_Enum).toInt()) - ((signed int)DayNow_Time) > 0) {
    EnergyHourly[24][3] = { 0 };
  }
}

/**(NOT USED)
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

// double EEMEM TotalEnergySincePowerUp_EEMEM[PowerArraySize] = { 0 };
// double EEMEM EnergyHourly_EEMEM[24][3] = { 0 };
// void ReadSensors_SaveEEPROM() {
//   eeprom_update_block(TotalEnergySincePowerUp, &TotalEnergySincePowerUp_EEMEM, 3 * sizeof(double));
//   for (int i = 0; (i < 500 || !eeprom_is_ready()); i++) {
//     delay(10);
//   }
//   eeprom_update_block(EnergyHourly, &EnergyHourly_EEMEM, 24 * 3 * sizeof(double));
// }

// void ReadSensors_RestoreFromEEprom() {
//   eeprom_read_block(TotalEnergySincePowerUp, &TotalEnergySincePowerUp_EEMEM, 3 * sizeof(double));
//   eeprom_read_block(EnergyHourly, &EnergyHourly_EEMEM, 24 * 3 * sizeof(double));
// }
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