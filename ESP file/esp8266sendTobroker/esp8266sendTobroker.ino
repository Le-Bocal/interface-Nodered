#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <PubSubClient.h>  

//WiFi Connection configuration
char ssid[] = "Usine";     //  le nom du reseau WIFI
char password[] = "nainporte";  // le mot de passe WIFI
//mqtt server
char mqtt_server[] = "192.20.2.106";  //adresse IP serveur 
#define MQTT_USER ""
#define MQTT_PASS ""

WiFiClient espClient;
PubSubClient MQTTclient(espClient);


//RFID Declaration
#include <SPI.h>
#include <MFRC522.h>
#define SS_PIN D8
#define RST_PIN D0
MFRC522 rfid(SS_PIN, RST_PIN); // Instance of the class
MFRC522::MIFARE_Key key;
// Init array that will store new NUID
byte nuidPICC[4];


void setup() {
Serial.begin(115200);
  // Conexion WIFI
   WiFi.begin(ssid, password);
   Serial.println("");
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
   Serial.println("Connected");
   MQTTclient.setServer(mqtt_server, 1883);

  //RFID SETUP
  SPI.begin(); // Init SPI bus
  rfid.PCD_Init(); // Init MFRC522
  Serial.println();
  Serial.print(F("Reader :"));
  rfid.PCD_DumpVersionToSerial();
  for (byte i = 0; i < 6; i++) {
   key.keyByte[i] = 0xFF;
  }
  Serial.println();
  Serial.println(F("This code scan the MIFARE Classic NUID."));
  Serial.print(F("Using the following key:"));
   printHex(key.keyByte, MFRC522::MF_KEY_SIZE);

  
}

void loop() {
  static uint32_t  lastTimeMqtt= 0;
  // connect serveur MQTT
  if (!MQTTclient.connected()) {
    MQTTconnect();
  }

  //une fois que on est connecté au mqtt

   // Reset the loop if no new card present on the sensor/reader. This saves the entire process when idle.
   if ( ! rfid.PICC_IsNewCardPresent())
   return;
 // Verify if the NUID has been readed
 if ( ! rfid.PICC_ReadCardSerial())
   return;
 Serial.print(F("PICC type: "));
 MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
 Serial.println(rfid.PICC_GetTypeName(piccType));
 // Check is the PICC of Classic MIFARE type
 if (piccType != MFRC522::PICC_TYPE_MIFARE_MINI &&
     piccType != MFRC522::PICC_TYPE_MIFARE_1K &&
     piccType != MFRC522::PICC_TYPE_MIFARE_4K) {
   Serial.println(F("Your tag is not of type MIFARE Classic."));
   return;
 }
 if (rfid.uid.uidByte[0] != nuidPICC[0] ||
     rfid.uid.uidByte[1] != nuidPICC[1] ||
     rfid.uid.uidByte[2] != nuidPICC[2] ||
     rfid.uid.uidByte[3] != nuidPICC[3] ) {
   Serial.println(F("A new card has been detected."));
   // Store NUID into nuidPICC array
   for (byte i = 0; i < 4; i++) {
     nuidPICC[i] = rfid.uid.uidByte[i];
   }
   Serial.println(F("The NUID tag is:"));
   //envoi du code nuid sur le broker
   MQTTclient.publish("esp32/humidity", printHex(rfid.uid.uidByte, rfid.uid.size));
   Serial.println();
   Serial.print(F("In dec: "));
   printDec(rfid.uid.uidByte, rfid.uid.size);
   Serial.println();
 }
 else Serial.println(F("Card read previously."));
 // Halt PICC
 rfid.PICC_HaltA();
 // Stop encryption on PCD
 rfid.PCD_StopCrypto1();
  
}



void MQTTconnect() {

  while (!MQTTclient.connected()) {
      Serial.print("Attente  MQTT connection...");
      String clientId = "TestClient-";
      clientId += String(random(0xffff), HEX);

    // test connexion
    if (MQTTclient.connect(clientId.c_str(),"","")) {
      Serial.println("connected");

    } else {  // si echec affichage erreur
      Serial.print("ECHEC, rc=");
      Serial.print(MQTTclient.state());
      Serial.println(" nouvelle tentative dans 5 secondes");
      delay(5000);
    }
  }
}



// RFID FONCTION

char* printHex(byte *buffer, byte bufferSize) {
 char* out = new char[100];
 for (byte i = 0; i < bufferSize; i++) {
   //out[i] = buffer[i] < 0x10 ? '0' : ' ';
   out[i] = buffer[i];
 }
 return out;
}
/**
   Helper routine to dump a byte array as dec values to Serial.
*/
void printDec(byte *buffer, byte bufferSize) {
 for (byte i = 0; i < bufferSize; i++) {
   Serial.print(buffer[i] < 0x10 ? " 0" : " ");
   Serial.print(buffer[i], DEC);
 }
}
