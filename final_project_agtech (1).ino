//-------------------------------//
#include <Arduino.h>
#include <Wire.h>
#include "Adafruit_SHT31.h"
//------------------------------------------------------------------
//Thingspeak:
// #include <ESP8266WiFi.h>
#include <WiFi.h>
//Thingspeak and wifi:
#include "ThingSpeak.h"
unsigned long myChannelNumber = 1708067;
const char * myWriteAPIKey = "987VFY0A6R2760SZ";

const char* ssid = "agrotech-lab-1"; // your wifi SSID name
const char* password = "1Afuna2Gezer" ;// wifi pasword
 
const char* server = "api.thingspeak.com";

WiFiClient client;
//------------------------------------------------------------------
Adafruit_SHT31 sht31 = Adafruit_SHT31();
  
//-------------------------------//
#define RELAY_PIN 4
 
void setup(){
  Serial.begin(9600);
  if (! sht31.begin(0x44)) {
    Serial.println("Couldn't find SHT31");
    while (1) delay(1);
  }
 //------------------------------------------------------------------
  //Thingspeak and wifi:
  WiFi.disconnect();
  delay(10);
  WiFi.begin(ssid, password);

  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  ThingSpeak.begin(client);
 
  WiFi.begin(ssid, password);
  
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("NodeMcu connected to wifi...");
  Serial.println(ssid);
  Serial.println(); 
//-------------------//
//relay setup
pinMode(RELAY_PIN,OUTPUT);
}
 
void loop(){  
 
float t = sht31.readTemperature();
float h = sht31.readHumidity();

if (! isnan(t))  {
    Serial.print("Temp *C = "); Serial.println(t);
  }
  else  {
    Serial.println("Failed to read temperature");
  }
  
  if (! isnan(h))  {
  Serial.print("Hum. % = "); Serial.println(h);
  }
  else  {
  Serial.println("Failed to read humidity");
  }
  
  Serial.println();
  delay(1000);
//----------------------------//
    //VPD Calculation
//VPD:
  int base1 = 10;
  float power1 = (7.5*t)/(237.3+t);
  float answer1 = pow(base1, power1);
  
  float P_sat = 610.7*answer1;
  float VPD_pascal = (P_sat*(100-h))/100;
  float VPD = VPD_pascal/1000;
  if (! isnan(VPD))  {
    Serial.print("VPD Kp = "); Serial.println(VPD);
  }
//----------------------------//
//RELAY_PIN open and close
if (VPD>1.5){
  Serial.println("VPD higher than 1.5  Relay close ");
digitalWrite(RELAY_PIN, LOW);//open the valve
delay(5000);
digitalWrite(RELAY_PIN, HIGH);//close the valve
delay(100);
}
else{
digitalWrite(RELAY_PIN, HIGH);//close the valve
Serial.println("VPD lower than 1.5  Relay open ");

}
//----------------------------//
// Check WIFI connection
\
    if (WiFi.status() != WL_CONNECTED) {
        WiFi.begin(ssid, password);
        Serial.println();
        Serial.println();
        Serial.print("Connecting to ");
        Serial.println(ssid);
        
            ThingSpeak.begin(client);

        WiFi.begin(ssid, password);
    }
 //----------------------------//   
 //Thingspeak:
  ThingSpeak.setField(1,t); // SHT temp
  ThingSpeak.setField(2,h); // SHT humidity
  ThingSpeak.setField(3,VPD); // VPD in kilopascal
  ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
  Serial.println("uploaded to Thingspeak server....");

  client.stop();
 
  Serial.println("Waiting to upload next reading...");
  Serial.println();
  
delay(120000);
}
