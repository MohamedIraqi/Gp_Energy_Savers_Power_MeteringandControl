/*
Date:26/4/2024
Created By Mohamed Iraqi
@author Mohamed Iraqi - M.A.Iraqi.Amen@gmail.com
This file implements Core functions for Communication with The arduino Code side of the code
*/

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <Firebase_ESP_Client.h>
#include <addons/TokenHelper.h>
#include <TimeLib.h>
#include <LiquidCrystal_I2C.h>

//I2C pins declaration
// Initialize the LiquidCrystal_I2C library with the LCD address and dimensions
#define lcdColumns 16
#define lcdRows 2
#define lcdAddress 0x27  // You may need to adjust this address based on your LCD module

// Insert your network credentials
#define WIFI_SSID "WIFI_SSID"
#define WIFI_PASSWORD "WIFI_PASSWORD"

// Assign the user sign in credentials
#define USER_EMAIL "a@a.com"
#define USER_PASSWORD "123456"

#define FIREBASE_API_KEY "AIzaSyDZlWYdjADT9hdkr-ZJpPW5Ildce_0Y-FU"

//Define Comm Phrases
#define Message_Ended "DataReceived"
#define Terminator_Char "$"

// NTP Servers:
static const char ntpServerName[] = "pool.ntp.org";
const int timeZone = 3;  // Central European Time(+2 and +1 for summer time)
void sendNTPpacket(IPAddress &address);
time_t getNtpTime();


WiFiUDP Udp;
unsigned int localPort = 8888;  // local port to listen for UDP packets

// Set the LCD address and dimensions
LiquidCrystal_I2C lcd(lcdAddress, lcdColumns, lcdRows, LCD_5x10DOTS);

// Define Firebase objects
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

String ProjectId = "grad-prototype1";
String DatabaseId = "(default)";
String collectionId = "Devices/";
bool taskCompleted = false;
String uid;

/**
     * Init for the Wifi and getting tokens for the firebase
     *
     * @param . void.
     * @return void
     *
     */
void ArdCom_Init() {
  Serial.begin(115200);
  //Connecting to wifi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();
  Serial.println("Starting UDP");
  Udp.begin(localPort);
  Serial.print("Local port: ");
  Serial.println(Udp.localPort());
  Serial.println("waiting for sync");
  setSyncProvider(getNtpTime);
  setSyncInterval(300);

  /* Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback;  // see addons/TokenHelper.h

  /* Assign the api key (required) */
  config.api_key = FIREBASE_API_KEY;

  /* Assign the user sign in credentials */
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;

  // Assign the maximum retry of token generation
  config.max_token_generation_retry = 9;

  Firebase.reconnectWiFi(true);
  fbdo.setBSSLBufferSize(4096 /* Rx buffer size in bytes from 512 - 16384 */, 1024 /* Tx buffer size in bytes from 512 - 16384 */);  //4096,1024
  fbdo.setResponseSize(2048);

  Firebase.begin(&config, &auth);
  //----------------------------------------------
  // Getting the user UID might take a few seconds
  //-----------------------------------------------
  Serial.println("Getting User UID");
  while ((auth.token.uid) == "") {
    Serial.print('.');
    delay(1000);
  }

  //-----------------
  // Print user UID
  //------------------
  uid = auth.token.uid.c_str();
  Serial.print("User UID: ");
  Serial.println(uid);

}

enum CommEnum {
  hour_Enum = 92,
  minute_Enum,
  second_Enum,
  day_Enum,
  weekday_Enum,
  month_Enum,
  year_Enum,
  now_Enum,
  Power_Enum,
  EnergyNow_Enum,
  EnergyHourly_Enum,
  TotalEnergySincePowerUp_Enum,
  IsConnected_Enum,
  Message_Ended_Enum
};

/**
     * Manages communication with the arduino
     *
     * @param . void.
     * @return void
     *
     */
void ArdCom_Com_Handler() {
  int Hour_TimeNow_VarF = hour();
  double EnergyHourly_Holder[EnergyHourlyArraySize][PowerArraySize] = { 0 };
  String ReceivedDataStringBUFFER = "";
  bool DataReceived = false;
  for (int i = 0; (i < 5000 && !DataReceived); i++) {
    if (Serial.available() > 0) {
      ReceivedDataStringBUFFER = Serial.readStringUntil('$');
      /*Time Conversation sending time as per request*/
      switch (ReceivedDataStringBUFFER.toInt()) {

        case hour_Enum:
          {
            Serial.print(hour());
            Serial.flush();
            Serial.print(Terminator_Char);
            Serial.flush();
            break;
          }
        case minute_Enum:
          {
            Serial.print(minute());
            Serial.flush();
            Serial.print(Terminator_Char);
            Serial.flush();
            break;
          }
        case second_Enum:
          {
            Serial.print(second());
            Serial.flush();
            Serial.print(Terminator_Char);
            Serial.flush();
            break;
          }
        case day_Enum:
          {
            Serial.print(day());
            Serial.flush();
            Serial.print(Terminator_Char);
            Serial.flush();
            break;
          }
        case weekday_Enum:
          {
            Serial.print(weekday());
            Serial.flush();
            Serial.print(Terminator_Char);
            Serial.flush();
            break;
          }
        case month_Enum:
          {
            Serial.print(month());
            Serial.flush();
            Serial.print(Terminator_Char);
            Serial.flush();
            break;
          }
        case year_Enum:
          {
            Serial.print(year());
            Serial.flush();
            Serial.print(Terminator_Char);
            Serial.flush();
            break;
          }
        case now_Enum:
          {
            Serial.print(now());
            Serial.flush();
            Serial.print(Terminator_Char);
            Serial.flush();
            break;
          }
        case Power_Enum:
          {
            for (int i = 0; i < PowerArraySize; i++) {

              if (Serial.available() == 0) {
                for (int i = 0; (i < 50 && (Serial.available() == 0)); i++) {
                  delay(5);
                }
              }
              ReceivedDataStringBUFFER = Serial.readStringUntil('$');
              power[i] = ReceivedDataStringBUFFER.toDouble();
            }
            break;
          }

        case EnergyNow_Enum:
          {
            for (int i = 0; i < PowerArraySize; i++) {

              if (Serial.available() == 0) {
                for (int i = 0; (i < 50 && (Serial.available() == 0)); i++) {
                  delay(5);
                }
              }
              ReceivedDataStringBUFFER = Serial.readStringUntil('$');
              EnergyNow[i] = ReceivedDataStringBUFFER.toDouble();
            }
            break;
          }
        case TotalEnergySincePowerUp_Enum:
          {
            for (int i = 0; i < PowerArraySize; i++) {

              if (Serial.available() == 0) {
                for (int i = 0; (i < 50 && (Serial.available() == 0)); i++) {
                  delay(5);
                }
              }
              ReceivedDataStringBUFFER = Serial.readStringUntil('$');
              TotalEnergySincePowerUp[i] = ReceivedDataStringBUFFER.toDouble();
            }
            break;
          }
        case EnergyHourly_Enum:
          {
            for (int g = 0; g < PowerArraySize; g++) {
              if (Serial.available() == 0) {
                for (int i = 0; (i < 50 && (Serial.available() == 0)); i++) {
                  delay(5);
                }
              }
              ReceivedDataStringBUFFER = Serial.readStringUntil('$');
              EnergyHourly[Hour_TimeNow_VarF][g] = ReceivedDataStringBUFFER.toDouble();
            }

            break;
          }
        case IsConnected_Enum:
          {
            Serial.print("YesConnected");
            Serial.flush();
            Serial.print(Terminator_Char);
            Serial.flush();
            break;
          }
        case Message_Ended_Enum:
          {
            if (Serial.available() == 0) {
              for (int i = 0; (i < 50 && (Serial.available() == 0)); i++) {
                delay(5);
              }
            };
            ReceivedDataStringBUFFER = Serial.readStringUntil('$');
            if (ReceivedDataStringBUFFER == Message_Ended) {
              DataReceived = true;
              Serial.flush();
            }
            break;
          }
      }
    }
  }
}

#pragma region UploadingData

// Function to get the document path based on PowerDeviceIter
String getDocumentPath(int PowerDeviceIter) {
  switch (PowerDeviceIter) {
    case 0:
      return (String) "Devices/" + "PnSJUwXfXIK9rSpS5wOJ" + "/";
    case 1:
      return (String) "Devices/" + "WrcFGbEeBnWHsg808zrp" + "/";
    case 2:
      return (String) "Devices/" + "jSZw18sdekWlBtibCqcOkkk67Rf2" + "/";
    case 3:
      return (String) "Devices/" + "HTkBdiC2CKcJRAe1empV" + "/";
    default:
      return "";
  }
}

// Function to upload single-value variables
void uploadDeviceVariables(int PowerDeviceIter, String DocumentPath) {
  FirebaseJson jsonData;

  // Add TotalEnergySincePowerUp
  jsonData.set("fields/TotalConsumption/doubleValue", TotalEnergySincePowerUp[PowerDeviceIter]);

  // Add current Power
  jsonData.set("fields/CurrentPower/doubleValue", power[PowerDeviceIter]);

  // Calculate and add SumEnergyHourly
  double SumEnergyHourly = 0;
  for (int i = 0; i < EnergyHourlyArraySize; i++) {
    SumEnergyHourly += EnergyHourly[i][PowerDeviceIter];
  }
  jsonData.set("fields/TotalPowerDay/doubleValue", SumEnergyHourly);

  // Set the update mask
  String updateMask = "TotalConsumption,CurrentPower,TotalPowerDay";

  // Patch the document
  Serial.print("Patching variables... ");
  if (Firebase.Firestore.patchDocument(&fbdo, ProjectId.c_str(), DatabaseId.c_str(), DocumentPath.c_str(), jsonData.raw(), updateMask.c_str())) {
    Serial.println("Variables patched to Firestore successfully");
  } else {
    Serial.print("Firestore patch failed: ");
    Serial.println(fbdo.errorReason());
  }
}

void uploadDeviceVariablesHouse(String DocumentPath) {
  FirebaseJson jsonData;

  // Calculate Total Energy Consumption for all devices
  double totalEnergyConsumption = 0;
  for (int i = 0; i < PowerArraySize; i++) {
    totalEnergyConsumption += TotalEnergySincePowerUp[i];
  }
  jsonData.set("fields/TotalConsumption/doubleValue", totalEnergyConsumption);

  // Calculate Total Current Power for all devices
  double totalCurrentPower = 0;
  for (int i = 0; i < PowerArraySize; i++) {
    totalCurrentPower += power[i];
  }
  jsonData.set("fields/CurrentPower/doubleValue", totalCurrentPower);

  // Calculate Total Power for the Day for all devices
  double totalPowerDay = 0;
  for (int i = 0; i < EnergyHourlyArraySize; i++) {
    for (int j = 0; j < PowerArraySize; j++) {
      totalPowerDay += EnergyHourly[i][j];
    }
  }
  jsonData.set("fields/TotalPowerDay/doubleValue", totalPowerDay);

  // Calculate and add SumEnergyHourly for the house
  double houseSumEnergyHourly = 0;
  for (int i = 0; i < EnergyHourlyArraySize; i++) {
    for (int j = 0; j < PowerArraySize; j++) {
      houseSumEnergyHourly += EnergyHourly[i][j];
    }
  }

  // Set the update mask
  String updateMask = "TotalConsumption,CurrentPower,TotalPowerDay";

  // Patch the document
  Serial.print("Patching house House variables... ");
  delay(999);
  if (Firebase.Firestore.patchDocument(&fbdo, ProjectId.c_str(), DatabaseId.c_str(), DocumentPath.c_str(), jsonData.raw(), updateMask.c_str())) {
    Serial.println("House variables patched to Firestore successfully");
  } else {
    Serial.print("Firestore patch failed: ");
    Serial.println(fbdo.errorReason());
  }
}

// Function to upload the hourly array
void ArdCom_UploadDataHourlyArray(int PowerDeviceIter, String DocumentPath) {
  FirebaseJson jsonData;
  FirebaseJsonArray hourlyArray;
  FirebaseJson myjson;

  // Add EnergyHourly as an array
  for (int i = 0; i < EnergyHourlyArraySize; i++) {
    hourlyArray.add(myjson.set("doubleValue", EnergyHourly[i][PowerDeviceIter]));
  }
  jsonData.set("fields/PowerDay/arrayValue/values", hourlyArray);

  // Set the update mask
  String updateMask = "PowerDay";

  // Patch the document
  Serial.print("Patch document... ");
  Serial.println();
  Serial.println(jsonData.raw());
  delay(999);
  if (Firebase.Firestore.patchDocument(&fbdo, ProjectId.c_str(), DatabaseId.c_str(), DocumentPath.c_str(), jsonData.raw(), updateMask.c_str())) {
    Serial.println("Data patched to Firestore successfully");
  } else {
    Serial.print("Firestore patch failed: ");
    Serial.println(fbdo.errorReason());
  }
}

void ArdCom_UploadDataHourlyArrayHouse(String DocumentPath) {
  FirebaseJson jsonData;
  FirebaseJsonArray hourlyArray;
  FirebaseJson myjson;

  // Calculate hourly energy for the house for each hour
  for (int hour = 0; hour < EnergyHourlyArraySize; hour++) {
    double hourlyEnergyHouse = 0;
    for (int device = 0; device < PowerArraySize; device++) {
      hourlyEnergyHouse += EnergyHourly[hour][device];
    }
    hourlyArray.add(myjson.set("integerValue", hourlyEnergyHouse));
  }

  // Add the hourly array to the JSON data
  jsonData.set("fields/PowerDay/arrayValue/values", hourlyArray);

  // Set the update mask
  String updateMask = "PowerDay";

  // Patch the document
  Serial.print("Patching hourly array for house... ");
  delay(999);
  if (Firebase.Firestore.patchDocument(&fbdo, ProjectId.c_str(), DatabaseId.c_str(), DocumentPath.c_str(), jsonData.raw(), updateMask.c_str())) {
    Serial.println("Hourly array for house patched to Firestore successfully");
  } else {
    Serial.print("Firestore patch failed: ");
    Serial.println(fbdo.errorReason());
  }
}


int mGetVarIt() {
  String DocumentPath = "Devices/PrivateVars/";
  FirebaseJsonData jsonData;  // For storing the retrieved field data
  FirebaseJson json;          // For parsing and creating JSON

  if (Firebase.Firestore.getDocument(&fbdo, ProjectId.c_str(), DatabaseId.c_str(), DocumentPath.c_str())) {
    Serial.printf("ok\n%s\n\n", fbdo.payload().c_str());
  } else {
    Serial.println(fbdo.errorReason());
    return -1;  // Return an error value
  }

  // Parse the JSON payload
  json.setJsonData(fbdo.payload());

  // Get m_PrivateIterator, storing the result in jsonData
  if (json.get(jsonData, "fields/m_PrivateIterator/integerValue")) {
    // Get the integer value from jsonData
    int privateIterator = jsonData.intValue;

    // Update the iterator
    if (privateIterator <= 7) {
      privateIterator++;
    } else {
      privateIterator = 0;
    }

    // Update the JSON for patching
    json.set("fields/m_PrivateIterator/integerValue", privateIterator);

    // Set the update mask
    String updateMask = "m_PrivateIterator";

    // Patch the document
    if (Firebase.Firestore.patchDocument(&fbdo, ProjectId.c_str(), DatabaseId.c_str(), DocumentPath.c_str(), json.raw(), updateMask.c_str())) {
      Serial.println("privateIt patched to Firestore successfully");
    } else {
      Serial.print("Firestore patch privateIt failed: ");
      Serial.println(fbdo.errorReason());
    }

    return privateIterator;  // Return the updated iterator value

  } else {
    Serial.println("Failed to get m_PrivateIterator field.");
    return -1;  // Return an error value
  }
}

// Main upload function
void ArdCom_UploadData() {
  String DocumentPath;
  Serial.end();
  switch (mGetVarIt()) {
    case 0:
      {
        DocumentPath = getDocumentPath(0);
        Serial.println("0** ");
        uploadDeviceVariables(0, DocumentPath);
        ESP.restart();
        break;
      }
    case 1:
      {
        DocumentPath = getDocumentPath(0);
        Serial.println("1** ");
        ArdCom_UploadDataHourlyArray(0, DocumentPath);
        ESP.restart();
        break;
      }
    case 2:
      {
        DocumentPath = getDocumentPath(1);
        Serial.println("2** ");
        uploadDeviceVariables(1, DocumentPath);
        ESP.restart();
        break;
      }
    case 3:
      {
        DocumentPath = getDocumentPath(1);
        Serial.println("3** ");
        ArdCom_UploadDataHourlyArray(1, DocumentPath);
        ESP.restart();
        break;
      }
    case 4:
      {
        DocumentPath = getDocumentPath(2);
        Serial.println("4** ");
        uploadDeviceVariables(2, DocumentPath);
        ESP.restart();
        break;
      }
    case 5:
      {
        DocumentPath = getDocumentPath(2);
        Serial.println("5** ");
        ArdCom_UploadDataHourlyArray(2, DocumentPath);
        ESP.restart();
        break;
      }
    case 6:
      {
        DocumentPath = getDocumentPath(3);
        Serial.println("6** ");
        uploadDeviceVariablesHouse( DocumentPath);
        ESP.restart();
        break;
      }
          case 7:
      {
        DocumentPath = getDocumentPath(3);
        Serial.println("7** ");
        ArdCom_UploadDataHourlyArrayHouse( DocumentPath);
        ESP.restart();
        break;
      }
    case 8:
      //delay(9000);
      break;
    default:
      //delay(9000);
      break;
  }
}
#pragma endregion UploadingData

/**
     * Updates lcd with data
     *
     * @param . void.
     * @return void
     *
     */
void ArdCom_lcd_DataDisp() {

  lcd.begin();      //Defining 16 columns and 2 rows of lcd display
  lcd.backlight();  //To Power ON the back light

  lcd.print(":KwH room 1:" + String(EnergyHourly[ArdCom_GetHour()][0]));
  delay(1000);
  lcd.clear();
  lcd.print("KwH room 2:" + String(EnergyHourly[ArdCom_GetHour()][1]));
  delay(1000);
  lcd.clear();
  lcd.print("KwH room 3:" + String(EnergyHourly[ArdCom_GetHour()][2]));
  delay(1000);
  lcd.clear();
  
  lcd.print("Total Room 1:" + String(TotalEnergySincePowerUp[0]));
  delay(1000);
  lcd.clear();
  lcd.print("Total Room 2:" + String(TotalEnergySincePowerUp[1]));
  delay(1000);
  lcd.clear();
  lcd.print("Total Room 3:" + String(TotalEnergySincePowerUp[2]));
  delay(1000);
  lcd.clear();

  lcd.print("Power Room 1:" + String(power[0]));
  delay(1000);
  lcd.print("Power Room 2:" + String(power[1]));
  delay(1000);
  lcd.print("Power Room 3:" + String(power[2]));
  delay(1000);

}



/**
     * Gets current hour
     *
     * @param . void.
     * @return returns current hour
     *
     */
int ArdCom_GetHour() {
  return hour();
}

/*-------- NTP code ----------*/

const int NTP_PACKET_SIZE = 48;      // NTP time is in the first 48 bytes of message
byte packetBuffer[NTP_PACKET_SIZE];  //buffer to hold incoming & outgoing packets

time_t getNtpTime() {
  IPAddress ntpServerIP;  // NTP server's ip address

  while (Udp.parsePacket() > 0)
    ;  // discard any previously received packets
  Serial.println("Transmit NTP Request");
  // get a random server from the pool
  WiFi.hostByName(ntpServerName, ntpServerIP);
  Serial.print(ntpServerName);
  Serial.print(": ");
  Serial.println(ntpServerIP);
  sendNTPpacket(ntpServerIP);
  uint32_t beginWait = millis();
  while (millis() - beginWait < 1500) {
    int size = Udp.parsePacket();
    if (size >= NTP_PACKET_SIZE) {
      Serial.println("Receive NTP Response");
      Udp.read(packetBuffer, NTP_PACKET_SIZE);  // read packet into the buffer
      unsigned long secsSince1900;
      // convert four bytes starting at location 40 to a long integer
      secsSince1900 = (unsigned long)packetBuffer[40] << 24;
      secsSince1900 |= (unsigned long)packetBuffer[41] << 16;
      secsSince1900 |= (unsigned long)packetBuffer[42] << 8;
      secsSince1900 |= (unsigned long)packetBuffer[43];
      return secsSince1900 - 2208988800UL + timeZone * SECS_PER_HOUR;
    }
  }
  Serial.println("No NTP Response :-(");
  return 0;  // return 0 if unable to get the time
}

// send an NTP request to the time server at the given address
void sendNTPpacket(IPAddress &address) {
  // set all bytes in the buffer to 0
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  packetBuffer[0] = 0b11100011;  // LI, Version, Mode
  packetBuffer[1] = 0;           // Stratum, or type of clock
  packetBuffer[2] = 6;           // Polling Interval
  packetBuffer[3] = 0xEC;        // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12] = 49;
  packetBuffer[13] = 0x4E;
  packetBuffer[14] = 49;
  packetBuffer[15] = 52;
  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:
  Udp.beginPacket(address, 123);  //NTP requests are to port 123
  Udp.write(packetBuffer, NTP_PACKET_SIZE);
  Udp.endPacket();
}
