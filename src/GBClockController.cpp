#include <GBClockController.h>
#include <Arduino.h>

#define minPWMFrequencySecond 30
#define maxPWMFrequencySecond 1000
#define minPWMFrequencyMinute 30
#define maxPWMFrequencyMinute 1000
#define minPWMFrequencyHour 35
#define maxPWMFrequencyHour 1015
#define maxSkippingSteps 50
#define timeBetweenMaxSteps 50
#define format_24H false
#define smoothSeconds true

#define pwmSecondStep (maxPWMFrequencySecond - minPWMFrequencySecond) / 60

GBClockController::GBClockController(uint8_t pinSecond, uint8_t pinMinute, uint8_t pinHour) {
    this->pinSecond = pinSecond;
    this->pinMinute = pinMinute;
    this->pinHour = pinHour;

    pinMode(pinSecond, OUTPUT);
    pinMode(pinMinute, OUTPUT);
    pinMode(pinHour, OUTPUT);
}

GBClockController::~GBClockController() {
    this->set(this->pinSecond, 0, &this->lastPWMSecond);
    this->set(this->pinMinute, 0, &this->lastPWMMinute);
    this->set(this->pinHour, 0, &this->lastPWMHour);
}

void GBClockController::updateTime(GBDateTime currentTime) {
    int mappedSeconds = map(currentTime.second, 0, 59, minPWMFrequencySecond, maxPWMFrequencySecond);
    if(smoothSeconds) {
        if (currentTime.second != this->lastSecond) {
            this->lastSecond = currentTime.second;
            millisOffset = millis();
        } else {
            int offset = (millis() - millisOffset);
            mappedSeconds += (pwmSecondStep * offset) / 1000;
        }
    }
    this->set(this->pinSecond,  mappedSeconds, &this->lastPWMSecond);
    this->set(this->pinMinute, map(currentTime.minute, 0, 59, minPWMFrequencyMinute, maxPWMFrequencyMinute), &this->lastPWMMinute);
    if (format_24H) {
        this->set(this->pinHour, map(currentTime.hour, 0, 23, minPWMFrequencyHour, maxPWMFrequencyHour), &this->lastPWMHour);
    } else {
        int hour = (12 + currentTime.hour-1) % 12;
        this->set(this->pinHour, map(hour, 0, 11, minPWMFrequencyHour, maxPWMFrequencyHour), &this->lastPWMHour);
    }
}

void GBClockController::turnOff() {
    this->set(this->pinSecond, minPWMFrequencySecond, &this->lastPWMSecond);
    this->set(this->pinMinute, minPWMFrequencyMinute, &this->lastPWMMinute);
    this->set(this->pinHour, minPWMFrequencyHour, &this->lastPWMHour);
}

void GBClockController::set(uint8_t pin, int16_t value, int16_t* lastValue) {

    int16_t difference = *lastValue - value;

    if (abs(difference) > maxSkippingSteps) {
        if (*lastValue > value) { // -
            *lastValue -= maxSkippingSteps;
        } else { // +
            *lastValue += maxSkippingSteps;
        }
        analogWrite(pin, *lastValue);
        delay(timeBetweenMaxSteps);
        set(pin, value, lastValue);
    } else {
        *lastValue = value;
        analogWrite(pin, value);
    }
}

void GBClockController::callibrate(int millisec) {
    Serial.println("Start callibrate");
    this->turnOff();
    delay(100);
    int minimum = max(minPWMFrequencyHour, max(minPWMFrequencyMinute, minPWMFrequencySecond));
    int maximum = min(maxPWMFrequencyHour, max(maxPWMFrequencyMinute, maxPWMFrequencySecond));
    for (int i=minimum; i < maximum; ++i) {

        this->set(this->pinSecond, i, &this->lastPWMSecond);
        this->set(this->pinMinute, i, &this->lastPWMMinute);
        this->set(this->pinHour, i, &this->lastPWMHour);
        delay(max((millisec/2) / (maximum - minimum), 1));
    }
    for (int i=maximum; i >= minimum; --i) {
        this->set(this->pinSecond, i, &this->lastPWMSecond);
        this->set(this->pinMinute, i, &this->lastPWMMinute);
        this->set(this->pinHour, i, &this->lastPWMHour);
        delay(max((millisec/2) / (maximum - minimum), 1));
    }
    this->turnOff();
    delay(100);
    Serial.println("End callibrate");
}