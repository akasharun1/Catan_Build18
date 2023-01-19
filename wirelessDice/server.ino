#include <ESP8266WiFi.h>
#include<SoftwareSerial.h>

SoftwareSerial serverSend(4, 5);  //D2, D1

const char* ssid = "rashi";
const char* password = "1234567r";
WiFiServer server(80);
int dice_values[] = {0, 0};
int count = 0;

void setup() {
  Serial.begin(9600);
  serverSend.begin(9600);
  // Connect to WiFi network
  Serial.println();
  Serial.println();
  Serial.print(F("connecting to "));
  Serial.println(ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(F("."));
  }
  Serial.println();
  Serial.println(F("WiFi connected"));
  // Start the server
  server.begin();
  Serial.println(F("server started"));
  // Print the IP address
  Serial.println(WiFi.localIP());
}
void loop() {
  // Check if a client has connected
  WiFiClient client = server.accept();
  if (!client) { return; }
  Serial.println(F("new client"));
  client.setTimeout(5000);  // default is 1000
  // Read the first line of the request
  String req = client.readStringUntil('\r');
  Serial.println(F("dice rolled:"));
  Serial.println(req);
  while (client.available()) {
    client.read();
  }
  client.print("dice rolled: "+req);
  int roll = req.toInt();
  int index = count%2;
  dice_values[index] = roll;
  int total_roll = -1;
  if (index) {
    total_roll = dice_values[0] + dice_values[1];
    Serial.println("total roll");
    Serial.println(total_roll);
    // add casing for when -1 no send
    serverSend.write(total_roll);
  }
  Serial.println(F("disconnecting from client"));
  count++;
}
