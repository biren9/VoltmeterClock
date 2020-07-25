#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>    
#include <GBTime.h>
#include <GBClockController.h>

#define PIN_WLAN_STATUS 16
#define PIN_CLOCK_SECOND 15
#define PIN_CLOCK_MINUTE 14
#define PIN_CLOCK_HOUR 12

WiFiManager wifiManager;
GBTime *timeService = new GBTime();
GBClockController *clockController = new GBClockController(PIN_CLOCK_SECOND, PIN_CLOCK_MINUTE, PIN_CLOCK_HOUR);

void setup(){
  Serial.begin(9600);
  pinMode(PIN_WLAN_STATUS, OUTPUT);
  wifiManager.autoConnect("VoltmeterClock", "11111111");
  
  do {
    clockController->callibrate(500);
  } while ( WiFi.status() != WL_CONNECTED );
  timeService->begin();
  GBDateTime currentTime = timeService->currentTime();
  Serial.printf("Time: %d.%d.%d   %d:%d:%d\n", currentTime.day, currentTime.month, currentTime.year, currentTime.hour, currentTime.minute, currentTime.second);
  Serial.end();
}

void loop() {
  if ( WiFi.status() != WL_CONNECTED ) {
    digitalWrite(PIN_WLAN_STATUS, LOW);
  } else {
    digitalWrite(PIN_WLAN_STATUS, HIGH);
  }

  GBDateTime currentTime = timeService->currentTime();
  clockController->updateTime(currentTime);
  delay(clockController->timoutBetweenUpdates());
}