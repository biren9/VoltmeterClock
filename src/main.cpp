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

GBTime *timeService;
GBClockController *clockController;

void setup() {
  Serial.begin(9600);
  WiFiManager *wifiManager = new WiFiManager();
  timeService = new GBTime(wifiManager);
  clockController = new GBClockController(PIN_CLOCK_SECOND, PIN_CLOCK_MINUTE, PIN_CLOCK_HOUR);
  pinMode(PIN_WLAN_STATUS, OUTPUT);
  WiFi.setSleepMode(WIFI_LIGHT_SLEEP);
  timeService->begin();
  do {
    clockController->callibrate(500);
  } while (WiFi.status() != WL_CONNECTED);
  GBDateTime currentTime = timeService->currentTime();
  Serial.printf("Time: %d.%d.%d   %d:%d:%d\n", currentTime.day, currentTime.month, currentTime.year, currentTime.hour, currentTime.minute, currentTime.second);
}

void loop() {
  digitalWrite(PIN_WLAN_STATUS, WiFi.status() == WL_CONNECTED ? HIGH : LOW);

  GBDateTime currentTime = timeService->currentTime();
  clockController->updateTime(currentTime);
  delay(clockController->timoutBetweenUpdates());
}