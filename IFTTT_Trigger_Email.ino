#include <WiFiNINA.h>
#include "secrets.h"
#include <Wire.h>
#include <BH1750FVI.h>

BH1750FVI LightSensor(BH1750FVI::k_DevModeContLowRes); // For light sensor

char ssid[] = SECRET_SSID;
char pass[] = SECRET_PASS;
int keyIndex = 0;            
WiFiClient  client;

char   HOST_NAME[] = "maker.ifttt.com";
String PATH_NAME   = "https://maker.ifttt.com/trigger/Arduino_Trigger/with/key/ja4JXBzjjIbqnBObGKN7s"; // change your EVENT-NAME and YOUR-KEY
String queryStringOn = "?value1=Tuned On";
String queryStringOff = "?value1=Tuned Off";

unsigned long TWO_HOUR_TIMER = 2 * 60 * 60 * 1000;
unsigned long TWENTY_FOUR_HOUR_TIMER = 24 * 60 * 60 * 1000;
unsigned long startTime;

bool ON = false;
bool OCCURED_TODAY = false;

void setup() {
  startTime = millis();

  Serial.begin(115200);      // Initialize serial 
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo native USB port only
  }
  
  LightSensor.begin();  

  // Connecting to the WIFI and printing IP address once connected
  int status = WL_IDLE_STATUS;
  while (status != WL_CONNECTED) {
    Serial.print("Connecting to ");
    Serial.println(ssid);
    status = WiFi.begin(ssid, pass);
    delay(5000);
  }
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());



  // connect to web server on port 80:
  if (client.connect(HOST_NAME, 80)) {
    // if connected:
    Serial.println("Connected to server");
  }
  else {// if not connected:
    Serial.println("connection failed");
  }
}

void loop() {
  uint16_t lux = LightSensor.GetLightIntensity();
  
  unsigned long TIME_ELAPSED = millis() - startTime; 
  unsigned long DAY_ELAPSED = millis() - startTime;

  if (lux >= 20000 && !ON) {
    // TURN OFF
    ON = true;

    OCCURED_TODAY = true;

    // make a HTTP request:
    // send HTTP header
    client.println("GET " + PATH_NAME + queryStringOn + " HTTP/1.1");
    client.println("Host: " + String(HOST_NAME));
    client.println("Connection: close");
    client.println();


    while (client.connected()) {
      if (client.available()) {
        // read an incoming byte from the server and print it to serial monitor:
        char c = client.read();
        Serial.print(c);
      }
    }
  }

  if (ON && TIME_ELAPSED >= TWO_HOUR_TIMER){
    // Turn off
    ON = false;

    // make a HTTP request:
    // send HTTP header
    client.println("GET " + PATH_NAME + queryStringOff + " HTTP/1.1");
    client.println("Host: " + String(HOST_NAME));
    client.println("Connection: close");
    client.println();


    while (client.connected()) {
      if (client.available()) {
        // read an incoming byte from the server and print it to serial monitor:
        char c = client.read();
        Serial.print(c);
      }
    }
  }

  if (DAY_ELAPSED >= TWENTY_FOUR_HOUR_TIMER){
    OCCURED_TODAY = false;
    startTime = millis();
  }
}

