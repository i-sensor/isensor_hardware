#include <LiquidCrystal_I2C.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <SFE_BMP180.h>
#include "SparkFunHTU21D.h"
#include <Wire.h>


#define ALTITUDE 254.0 // Altitude in meters
#define SERVER_IP "https://3a1e-46-211-150-119.ngrok.io"
#define SENSORPIN A0

#ifndef STASSID
#define STASSID "totolink_2.4G"
#define STAPSK "63422955"
#endif

HTU21D myHumidity;    //Humidity sensor
SFE_BMP180 pressure;  //Pressure/Temperature sensor
LiquidCrystal_I2C lcd(0x27, 16, 2);

void startBMP180();
void startWifi();

String measureTemperature();
String measurePressure();
String measureHumidity();
String measureUV();

void sendJson(String temp, String hum, String bar, String lux);


void setup() {
  Serial.begin(9600);
  
  lcd.begin();
  lcd.backlight();

  myHumidity.begin();
  startBMP180();
  startWifi();
}

void loop() {
  String st = measureTemperature();
  String sp = measurePressure();
  String sh = measureHumidity();
  String sl = measureUV();

  lcd.setCursor(0,0);
  lcd.print("T:" + st + " C    H:" + sh + "%");
  lcd.setCursor(0,1);
  lcd.print("P:" + sp + " bar  L:" + sl + "UV");
  
  delay(10000);
}

void sendJson(String temp, String hum, String bar, String lux)
{
    String jsonData = "{\"humidity\":\"" + hum + "\", \"temperature\":\"" + temp + "\", \"pressure\":\"" + bar + "\", \"light\":\"" + lux + "\"}";
    
    if ((WiFi.status() == WL_CONNECTED)) 
    {
      WiFiClient client;
      HTTPClient http;
      
      Serial.print("[HTTP] begin...\n");
      
      http.begin(client, SERVER_IP);  
      http.addHeader("Content-Type", "application/json");
      
      int httpCode = http.POST(jsonData);
      
      if (httpCode > 0) 
      {
        Serial.printf("[HTTP] POST... code: %d\n", httpCode);
        if (httpCode == HTTP_CODE_OK) 
        {
          const String& payload = http.getString();
          Serial.println("received payload:\n<<");
          Serial.println(payload);
          Serial.println(">>");
        }
      }
       
      else 
      {
      Serial.printf("[HTTP] POST... failed, error: %s\n", http.errorToString(httpCode).c_str());
      }
      
      http.end();
  }
}

String measureUV()
{
  int UV = analogRead(SENSORPIN);

  if(UV >=    0 && UV <   50) UV =  0;
  if(UV >=   50 && UV <  227) UV =  1;
  if(UV >=  227 && UV <  318) UV =  2;
  if(UV >=  318 && UV <  408) UV =  3;
  if(UV >=  408 && UV <  503) UV =  4;
  if(UV >=  503 && UV <  606) UV =  5;
  if(UV >=  606 && UV <  696) UV =  6;
  if(UV >=  696 && UV <  795) UV =  7;
  if(UV >=  795 && UV <  881) UV =  8;
  if(UV >=  881 && UV <  976) UV =  9;
  if(UV >=  976 && UV < 1079) UV = 10;
  if(UV >= 1079)              UV = 11;
  
  String lux = String(UV);

  return lux;
}

String measureHumidity()
{
  float humd = myHumidity.readHumidity();

  String hum = String(humd);

  return hum;
}

String measureTemperature()
{
  char status;
  double T,P;
  
  status = pressure.startTemperature();
  
  if (status != 0)
  {
    delay(status);
    status = pressure.getTemperature(T);
    
    if (status != 0)
    {
      status = pressure.startPressure(3);
      
      if (status != 0)
      {
        delay(status);
        status = pressure.getPressure(P,T);
        
        if (status = 0)
        {
          Serial.println("error retrieving pressure measurement\n");
        }
      }
      else Serial.println("error starting pressure measurement\n");
    }
    else Serial.println("error retrieving temperature measurement\n");
  }
  else Serial.println("error starting temperature measurement\n");

  String temp = String(T);

  return temp;
}

String measurePressure()
{
  char status;
  double T,P;
  
  status = pressure.startTemperature();
  
  if (status != 0)
  {
    delay(status);
    status = pressure.getTemperature(T);
    
    if (status != 0)
    {
      status = pressure.startPressure(3);
      
      if (status != 0)
      {
        delay(status);
        status = pressure.getPressure(P,T);
        
        if (status = 0)
        {
          Serial.println("error retrieving pressure measurement\n");
        }
      }
      else Serial.println("error starting pressure measurement\n");
    }
    else Serial.println("error retrieving temperature measurement\n");
  }
  else Serial.println("error starting temperature measurement\n");

  String bar = String(P);

  return bar;
}

void startBMP180()
{
  if (pressure.begin())
  {
    Serial.println("BMP180 init success");
  }
  else
  {
    Serial.println("BMP180 init fail\n\n");
    while(1); // Pause forever.
  }
}

void startWifi()
{
  WiFi.begin(STASSID, STAPSK);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected! IP address: ");
  Serial.println(WiFi.localIP());
}
