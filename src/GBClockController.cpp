#include <GBClockController.h>
#include <Arduino.h>

#define maxPWMFrequency 1023

GBClockController::GBClockController(uint8_t pinSecond, uint8_t pinMinute, uint8_t pinHour) {
    this->pinSecond = pinSecond;
    this->pinMinute = pinMinute;
    this->pinHour = pinHour;

    pinMode(pinSecond, OUTPUT);
    pinMode(pinMinute, OUTPUT);
    pinMode(pinHour, OUTPUT);
}

GBClockController::~GBClockController() {
    //
}

void GBClockController::updateTime(GBDateTime currentTime) {
    analogWrite(this->pinSecond, map(currentTime.second, 0, 59, 0, maxPWMFrequency));
    analogWrite(this->pinMinute, map(currentTime.minute, 0, 59, 0, maxPWMFrequency));
    analogWrite(this->pinHour, map(currentTime.hour, 0, 23, 0, maxPWMFrequency));
}

void GBClockController::turnOff() {
    analogWrite(this->pinSecond, 0);
    analogWrite(this->pinMinute, 0);
    analogWrite(this->pinHour, 0);
}

void GBClockController::callibrate(int millisec) {
    Serial.println("Start callibrate");
    this->turnOff();
    delay(100);
    for (int i=0; i < maxPWMFrequency; ++i) {
        analogWrite(this->pinSecond, i);
        analogWrite(this->pinMinute, i);
        analogWrite(this->pinHour, i);
        delay(max((millisec/2) / maxPWMFrequency, 1));
    }

    for (int i=maxPWMFrequency; i >= 0; --i) {
        analogWrite(this->pinSecond, i);
        analogWrite(this->pinMinute, i);
        analogWrite(this->pinHour, i);
        delay(max((millisec/2) / maxPWMFrequency, 1));
    }
    this->turnOff();
    delay(100);
    Serial.println("End callibrate");
}