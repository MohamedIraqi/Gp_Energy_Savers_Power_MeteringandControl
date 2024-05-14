/*
Date:26/4/2024
Created By Mohamed Iraqi
This file implements Core functions for Communication with The arduino Code side of the code
*/

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
//#include <Firebase_ESP_Client.h>
//#include <addons/TokenHelper.h>
#include <TimeLib.h>

// Insert your network credentials
#define WIFI_SSID "HAMBOZZA"
#define WIFI_PASSWORD "IhavetwoeyesandIcansee@724"

// Assign the user sign in credentials
#define USER_EMAIL "a@a.com"
#define USER_PASSWORD "123456"

//Define Comm Phrases
#define Message_Ended "DataReceived"
#define Terminator_Char "$"
// NTP Servers:
static const char ntpServerName[] = "pool.ntp.org";
const int timeZone = 3;  // Central European Time
void sendNTPpacket(IPAddress &address);
time_t getNtpTime();

WiFiUDP Udp;
unsigned int localPort = 8888;  // local port to listen for UDP packets


// Define Firebase objects
// FirebaseData fbdo;
// FirebaseAuth auth;
// FirebaseConfig config;
String projectId = "test1-123fe";
String databaseId = "(default)";
String collectionId = "users/";
String documentPath = "users/Wu0gxdSjnldkDVX6kas0Z5H8dby1";
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
  //Connecting to wifi
  Serial.begin(115200);
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
  // /* Assign the callback function for the long running token generation task */
  // config.token_status_callback = tokenStatusCallback;  // see addons/TokenHelper.h

  // /* Assign the api key (required) */
  // config.api_key = FIREBASE_API_KEY;

  // /* Assign the user sign in credentials */
  // auth.user.email = USER_EMAIL;
  // auth.user.password = USER_PASSWORD;

  // // Assign the maximum retry of token generation
  // config.max_token_generation_retry = 9;

  // Firebase.reconnectWiFi(true);
  // fbdo.setResponseSize(4096);
  // fbdo.setBSSLBufferSize(4096 /* Rx buffer size in bytes from 512 - 16384 */, 1024 /* Tx buffer size in bytes from 512 - 16384 */);

  // Firebase.begin(&config, &auth);
  // //----------------------------------------------
  // // Getting the user UID might take a few seconds
  // //-----------------------------------------------
  // Serial.println("Getting User UID");
  // while ((auth.token.uid) == "") {
  //   Serial.print('.');
  //   delay(1000);
  // }

  // //-----------------
  // // Print user UID
  // //------------------
  // uid = auth.token.uid.c_str();
  // Serial.print("User UID: ");
  // Serial.println(uid);
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
  double EnergyHourly_Holder[24][PowerArraySize] = { 0 };
  String ReceivedDataStringBUFFER = "";
  bool DataReceived = false;
  for (int i = 0; (i < 500 || !DataReceived); i++) {
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

/**
     * Checks wifi connection and sends "." in serial if offline
     *
     * @param . void.
     * @return ture if online or false if offline
     *
     */
bool ArdCom_StatusConnectedWifi() {
  bool status = true;
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }
  return status;
}

/**
     * Gets current hour
     *
     * @param . void.
     * @return returns current hour
     *
     */
int ArdCom_MyHour() {
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
