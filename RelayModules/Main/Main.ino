#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <Firebase_ESP_Client.h>
#include <addons/TokenHelper.h>
#include <ArduinoJson.h>

// Insert your network credentials
#define WIFI_SSID "BARAH SHARED 4"
#define WIFI_PASSWORD "BARAH16#*44"

/** Define Firebase credentials and project details **/
#define FIREBASE_PROJECT_ID "grad-prototype1"
#define PROJECT_LOCATION "eur3"
#define FIREBASE_API_KEY "AIzaSyDZlWYdjADT9hdkr-ZJpPW5Ildce_0Y-FU"
#define DATABASE_URL "https://grad-prototype1-default-rtdb.europe-west1.firebasedatabase.app/"
#define USER_EMAIL "a@a.com"
#define USER_PASSWORD "123456"

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;
String collectionId = "Devices/";
String DocumentPath1 = "Devices/WrcFGbEeBnWHsg808zrp";
String DocumentPath2 = "Devices/PnSJUwXfXIK9rSpS5wOJ/";
String ProjectId = "grad-prototype1";
String DatabaseId = "(default)";
bool taskCompleted = false;
String uid;

void setupFirebase() {
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

  config.database_url = DATABASE_URL;
  config.token_status_callback = tokenStatusCallback;
  config.api_key = FIREBASE_API_KEY;

  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;

  config.max_token_generation_retry = 9;

  Firebase.reconnectWiFi(true);
  fbdo.setResponseSize(4096);
  fbdo.setBSSLBufferSize(4096, 1024);

  Firebase.begin(&config, &auth);

  Serial.println("Firebase setup completed.");
}
/*
void firebaseOperations() {
  if (Firebase.isTokenExpired()) {
    Firebase.refreshToken(&config);
    Serial.println("Refresh token");
  }

  FirebaseJson content;

  String StringVluesss = "b";
  content.set("PrivateVars/adel/stringValue", StringVluesss.c_str());

  Serial.print("Create document... ");
  if (Firebase.Firestore.createDocument(&fbdo, FIREBASE_PROJECT_ID, databaseId.c_str(), DocumentPath1.c_str(), content.raw())) {
    Serial.printf("createdoc=\n%s\n\n", fbdo.payload().c_str());
  } else {
    Serial.println(fbdo.errorReason());
    Serial.println(content.raw());
  }


  Serial.print("Get a document... ");
  if (Firebase.Firestore.getDocument(&fbdo, FIREBASE_PROJECT_ID, databaseId.c_str(), DocumentPath1.c_str(), "")) {
    Serial.printf("GetDoc=\n%s\n\n", fbdo.payload().c_str());

    FirebaseJson payload;
    payload.setJsonData(fbdo.payload().c_str());

    FirebaseJsonData jsonData;
    Serial.print("Getting payload... ");
    Serial.println(payload.get(jsonData, "PrivateVars/integerValue", true));
    Serial.print("Getstringval ");
    Serial.println(jsonData.stringValue);

    if (payload.get(jsonData, "PrivateVars/OnOff")) {
      bool binaryValue = payload.get(jsonData, "PrivateVars/OnOff");
      
      if (!binaryValue) {
        Serial.write(0xA0);
        Serial.write(0x01);
        Serial.write(0x01);
        Serial.write(0xA2);    
      } else {
        Serial.write(0xA0);
        Serial.write(0x01);
        Serial.write(0x00);
        Serial.write(0xA1);  
      }
    } else {
      Serial.println("Failed to retrieve binary value from Firestore.");
    }
  } else {
    Serial.print("Get failed. ");
    Serial.println(fbdo.errorReason());
  }

  delay(5000);  // Delay between reads
}
/*
void ReadBOOl()
{
   String DocumentPath1 = "Devices/PrivateVars/";
  FirebaseJsonData jsonData;  // For storing the retrieved field data
  FirebaseJson json;          // For parsing and creating JSON

  if (Firebase.Firestore.getDocument(&fbdo, ProjectId.c_str(), DatabaseId.c_str(), DocumentPath1.c_str())) {
    Serial.printf("ok\n%s\n\n", fbdo.payload().c_str());
  } else {
    Serial.println(fbdo.errorReason());
  }

  // Parse the JSON payload
  json.setJsonData(fbdo.payload());

  // Get OnOffR1, storing the result in jsonData
  if (json.get(jsonData, "fields/OnOffR1/integerValue")) {
    // Get the integer value from jsonData
    int ROOM1 = jsonData.intValue;

    // Set the update mask
    String updateMask = "OnOffR1";

    // Patch the document
    if (Firebase.Firestore.patchDocument(&fbdo, ProjectId.c_str(), DatabaseId.c_str(), DocumentPath1.c_str(), json.raw(), updateMask.c_str())) {
      Serial.println("privateIt patched to Firestore successfully");
    } else {
      Serial.print("Firestore patch privateIt failed: ");
      Serial.println(fbdo.errorReason());
    }

      if (ROOM1==0) {
        Serial.write(0xA0);
        Serial.write(0x01);
        Serial.write(0x00);
        Serial.write(0xA1); 
      } else {
        Serial.write(0xA0);
        Serial.write(0x01);
        Serial.write(0x01);
        Serial.write(0xA2);     
      }



  } else {
    Serial.println("Failed to get OnOffR1 field.");
  }
}*/

void mGetVarIt() {
  String DocumentPath1 = "Devices/WrcFGbEeBnWHsg808zrp";
  FirebaseJsonData jsonData;  // For storing the retrieved field data
  FirebaseJson json;          // For parsing and creating JSON

  if (Firebase.Firestore.getDocument(&fbdo, ProjectId.c_str(), DatabaseId.c_str(), DocumentPath1.c_str())) {
    Serial.printf("ok\n%s\n\n", fbdo.payload().c_str());
  } else {
    Serial.println(fbdo.errorReason());
  }

  // Parse the JSON payload
  json.setJsonData(fbdo.payload());

  // Get On_Off, storing the result in jsonData
  if (json.get(jsonData, "fields/SwitchONOFF/integerValue")) {
    // Get the boolean value from jsonData
    int onOffState = jsonData.intValue;

    // Update the JSON for patching (optional, if you need to update the value)
    json.set("fields/SwitchONOFF/integerValue", onOffState);

    // Set the update mask (optional, if you are patching)
    String updateMask = "SwitchONOFF";

    // Patch the document (optional, if you are updating the value)
    if (Firebase.Firestore.patchDocument(&fbdo, ProjectId.c_str(), DatabaseId.c_str(), DocumentPath1.c_str(), json.raw(), updateMask.c_str())) {
      Serial.println("On_Off patched to Firestore successfully");
    } else {
      Serial.print("Firestore patch SwitchONOFF failed: ");
      Serial.println(fbdo.errorReason());
    }

    // Control your device based on the boolean value
    if (onOffState==0) {
      Serial.write(0xA0);
      Serial.write(0x01);
      Serial.write(0x01);
      Serial.write(0xA2);
    } else {
      Serial.write(0xA0);
      Serial.write(0x01);
      Serial.write(0x00);
      Serial.write(0xA1);
    }
  } else {
    Serial.println("Failed to get SwitchONOFF field.");
  }
}

void readR2() {
  String DocumentPath2 = "Devices/PnSJUwXfXIK9rSpS5wOJ/";
  FirebaseJsonData jsonData;  // For storing the retrieved field data
  FirebaseJson json;          // For parsing and creating JSON

  if (Firebase.Firestore.getDocument(&fbdo, ProjectId.c_str(), DatabaseId.c_str(), DocumentPath2.c_str())) {
    Serial.printf("ok\n%s\n\n", fbdo.payload().c_str());
  } else {
    Serial.println(fbdo.errorReason());
  }

  // Parse the JSON payload
  json.setJsonData(fbdo.payload());

  // Get m_PrivateIterator, storing the result in jsonData
  if (json.get(jsonData, "fields/SwitchONOFF/integerValue")) {
    // Get the integer value from jsonData
    int privateIterator = jsonData.intValue;


    // Update the JSON for patching
    json.set("fields/SwitchONOFF/integerValue", privateIterator);

    // Set the update mask
    String updateMask = "SwitchONOFF";

    // Patch the document
    if (Firebase.Firestore.patchDocument(&fbdo, ProjectId.c_str(), DatabaseId.c_str(), DocumentPath2.c_str(), json.raw(), updateMask.c_str())) {
      Serial.println("SwitchONOFF patched to Firestore successfully");
    } else {
      Serial.print("Firestore patch On_Off failed: ");
      Serial.println(fbdo.errorReason());
    }
    if (!(privateIterator == 0)) {
      Serial.write(0xA0);
      Serial.write(0x02);
      Serial.write(0x01);
      Serial.write(0xA3);
    } else {
      Serial.write(0xA0);
      Serial.write(0x02);
      Serial.write(0x00);
      Serial.write(0xA2);
    }




  } else {
    Serial.println("Failed to get m_PrivateIterator field.");
  }
}

void setup() {
  Serial.begin(115200);
  setupFirebase();
}
void loop() {
  Serial.println("------------------- START OF LOOP -------------------");
  Serial.print("WiFi status: "); Serial.println(WiFi.status());

  // firebaseOperations();
  Serial.print("mGetVarIt() start: "); Serial.println(millis());
  mGetVarIt();
  Serial.print("mGetVarIt() end: "); Serial.println(millis());
  Serial.print("Firebase error: "); Serial.println(fbdo.errorReason());

  Serial.println("------------------- END OF LOOP -------------------");
}
