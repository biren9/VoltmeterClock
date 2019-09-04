#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <GBTime.h>
#include <GBClockController.h>

#define PIN_WLAN_STATUS 16
#define PIN_CLOCK_SECOND 15
#define PIN_CLOCK_MINUTE 14
#define PIN_CLOCK_HOUR 12

const char *ssid =  "<SSID>";
const char *password = "<Password>";
GBTime *timeService = new GBTime();
GBClockController *clockController = new GBClockController(PIN_CLOCK_SECOND, PIN_CLOCK_MINUTE, PIN_CLOCK_HOUR);

void setup(){
  Serial.begin(9600);
  pinMode(PIN_WLAN_STATUS, OUTPUT);

  WiFi.begin(ssid, password);
  do {
    clockController->callibrate(500);
  } while ( WiFi.status() != WL_CONNECTED );
  timeService->begin();
}

void loop() {
  if ( WiFi.status() != WL_CONNECTED ) {
    digitalWrite(PIN_WLAN_STATUS, LOW);
  } else {
    digitalWrite(PIN_WLAN_STATUS, HIGH);
  }

  GBDateTime currentTime = timeService->currentTime();
  clockController->updateTime(currentTime);
 
  Serial.printf("Time: %d.%d.%d   %d:%d:%d\n", currentTime.day, currentTime.month, currentTime.year, currentTime.hour, currentTime.minute, currentTime.second);
  delay(500);
}