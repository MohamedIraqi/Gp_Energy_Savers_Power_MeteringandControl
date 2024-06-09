#include <ESP8266WiFi.h>
#include <Firebase_ESP_Client.h>
#include <addons/TokenHelper.h> // For token generation 
#include <SoftwareSerial.h>

// Wi-Fi Credentials
#define WIFI_SSID "no"
#define WIFI_PASSWORD "noo321Kno@"

// Firebase Credentials (replace with your actual values)
#define API_KEY "AIzaSyDZlWYdjADT9hdkr-ZJpPW5Ildce_0Y-FU"
#define USER_EMAIL "a@a.com"
#define USER_PASSWORD "123456"
#define FIREBASE_PROJECT_ID "grad-prototype1"

// Document Paths
const String houseDocPath = "/Devices/HTkBdiC2CKcJRAe1empV";
const String roomDocPaths[] = {
  "/Devices/PnSJUwXfXIK9rSpS5wOJ", 
  "/Devices/WrcFGbEeBnWHsg808zrp",
  "/Devices/jSZw18sdekWlBtibCqcOkkk67Rf2"
};

// Relay Control Commands (Hex)
// Relay Module 1 (Rooms 1 and 2)
const byte openRelay1Cmd[] = {0xA0, 0x01, 0x01, 0xA2}; 
const byte closeRelay1Cmd[] = {0xA0, 0x01, 0x00, 0xA1};
const byte openRelay2Cmd[] = {0xA0, 0x02, 0x01, 0xA3};
const byte closeRelay2Cmd[] = {0xA0, 0x02, 0x00, 0xA2};

// Relay Module 2 (Room 3)
const byte openRelay3Cmd[] = {0xA0, 0x01, 0x01, 0xA2}; 
const byte closeRelay3Cmd[] = {0xA0, 0x01, 0x00, 0xA1}; 

// Relay Module Serial Ports (Choose your GPIO pins!)
#define RELAY_MODULE_1_RX_PIN D2 // GPIO2 (NodeMCU)
#define RELAY_MODULE_1_TX_PIN D1 // GPIO1 (NodeMCU)
#define RELAY_MODULE_2_RX_PIN D6 // GPIO12 (NodeMCU)
#define RELAY_MODULE_2_TX_PIN D7 // GPIO13 (NodeMCU)

// Firebase Objects
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

// SoftwareSerial for Relay Modules
SoftwareSerial relayModule1Serial(RELAY_MODULE_1_RX_PIN, RELAY_MODULE_1_TX_PIN);
SoftwareSerial relayModule2Serial(RELAY_MODULE_2_RX_PIN, RELAY_MODULE_2_TX_PIN);

// Function to get "On_Off" status from Firestore
bool getOnOffStatus(const String& docPath) {
  FirebaseJson json;
  FirebaseJsonData jsonData;
Serial.print("here Error 7amada");
  if (Firebase.Firestore.getDocument(&fbdo, FIREBASE_PROJECT_ID, "(default)", docPath.c_str())) {
    Serial.print(fbdo.payload());
    json.setJsonData(fbdo.payload());
    if (json.get(jsonData, "fields/On_Off/booleanValue")) {
      return jsonData.boolValue;
    }
  } else {
    Serial.printf("Error getting document: %s\n", fbdo.errorReason().c_str());
  }
  return false; // Default to off in case of errors
}

// Function to control a relay (send command over SoftwareSerial)
void controlRelay(SoftwareSerial& serialPort, const byte* command, bool openRelay) {
  Serial.print(openRelay ? "Opening relay: " : "Closing relay: ");
  for (size_t i = 0; i < sizeof(command); i++) {
    Serial.print(command[i], HEX); Serial.print(" ");
    serialPort.write(command[i]);
  }
  Serial.println();
}

void setup() {
  Serial.begin(115200);

  // Connect to Wi-Fi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());

  // Initialize Firebase
  config.api_key = API_KEY;
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;
  config.token_status_callback = tokenStatusCallback; // See addons/TokenHelper.h

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);

  // Initialize SoftwareSerial for relay modules
  relayModule1Serial.begin(9600);
  relayModule2Serial.begin(9600);
}

void loop() {
  // Check for changes in Firestore every 5 seconds (adjust as needed)
  if (Firebase.ready() && millis() % 5000 == 0) {

    bool houseOnOff = getOnOffStatus(houseDocPath); 
    Serial.printf("House On_Off: %s\n", houseOnOff ? "true" : "false");

    for (int i = 0; i < 3; i++) {
      bool roomOnOff = getOnOffStatus(roomDocPaths[i]);
      Serial.printf("Room %d On_Off: %s\n", i + 1, roomOnOff ? "true" : "false");

      // AND the house and room statuses
      bool relayState = houseOnOff && roomOnOff; 

      // Control the relays based on the combined state
      if (i == 0) { // Room 1
        controlRelay(relayModule1Serial, relayState ? closeRelay1Cmd : openRelay1Cmd, !relayState);
      } else if (i == 1) { // Room 2
        controlRelay(relayModule1Serial, relayState ? closeRelay2Cmd : openRelay2Cmd, !relayState);
      } else if (i == 2) { // Room 3
        controlRelay(relayModule2Serial, relayState ? closeRelay3Cmd : openRelay3Cmd, !relayState);
      }
    }
  }
}