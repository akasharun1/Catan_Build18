#include <Wire.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>

#ifndef STASSID
#define STASSID "rashi"
#define STAPSK "1234567r"
#endif

// for gyro and accelerometer
const int MPU=0x68;
int16_t AcX, AcY, AcZ, Tmp, GyX, GyY, GyZ;

// for WiFi
const char* ssid = STASSID;
const char* password = STAPSK;
const char* host = "172.20.10.5";
const uint16_t port = 80;

ESP8266WiFiMulti WiFiMulti;

void setup() {
  //for gyro and accelerometer
  Wire.begin();
  Wire.beginTransmission(MPU);
  Wire.write(0x6B);
  Wire.write(0);
  Wire.endTransmission(true);
    
  Serial.begin(9600);

  // Connect to Wifi Network
  WiFi.mode(WIFI_STA);
  WiFiMulti.addAP(ssid, password);

  Serial.println();
  Serial.println();
  Serial.print("Wait for WiFi... ");

  while (WiFiMulti.run() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  delay(500);
}


void loop() {
  Wire.beginTransmission(MPU);
  Wire.write(0x3B);
  Wire.endTransmission(false);
  Wire.requestFrom(MPU,12,true);
  AcX=Wire.read()<<8|Wire.read();
  AcY=Wire.read()<<8|Wire.read();
  AcZ=Wire.read()<<8|Wire.read();
  GyX=Wire.read()<<8|Wire.read();
  GyY=Wire.read()<<8|Wire.read();
  GyZ=Wire.read()<<8|Wire.read();
  Serial.print("Accelerometer: ");
  Serial.print("X = "); Serial.print(AcX);
  Serial.print(" | Y = "); Serial.print(AcY);
  Serial.print(" | Z = "); Serial.println(AcZ);
  
  // Logic for determining if moving


  // Logic for determining dice side
  String diceSide;
  if (AcX > 12000) {
    diceSide = "1";  
  }
  else if (AcX < -12000) {
    diceSide = "6";  
  }
  else if (AcY > 12000) {
    diceSide = "5";  
  }
  else if (AcY < -12000) {
    diceSide = "2";  
  }
  else if (AcZ > 12000) {
    diceSide = "3";  
  }
  else if (AcZ < -12000) {
    diceSide = "4";  
  }
  // temporary replace with "unknown"
  else {
    diceSide = "unknown";  
  }
  
  // for Wifi
  Serial.print("connecting to ");
  Serial.print(host);
  Serial.print(':');
  Serial.println(port);

  // Use WiFiClient class to create TCP connections
  WiFiClient client;
  if (!client.connect(host, port)) {
    Serial.println("connection failed");
    Serial.println("wait 2 sec...");
    delay(2000);
    return;
  }

  // Send message to server
  Serial.print("Client Sending -  ");
  Serial.print(diceSide);
  Serial.println(" ");
  client.println(diceSide);

  // Recieve message from server
  Serial.println("receiving from remote server");
  String messageRecieved = client.readStringUntil('\r');
  Serial.print("From Host - ");
  Serial.print(messageRecieved);
  Serial.println(" ");

  Serial.println("closing connection");
  client.stop();
  
}
