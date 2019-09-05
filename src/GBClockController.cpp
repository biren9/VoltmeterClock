#include <GBClockController.h>
#include <Arduino.h>

#define minPWMFrequency 30
#define maxPWMFrequency 1000
#define maxSkippingSteps 50
#define timeBetweenMaxSteps 50
#define format_24H false

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
    this->set(this->pinSecond, map(currentTime.second, 0, 59, minPWMFrequency, maxPWMFrequency), &this->lastSecond);
    this->set(this->pinMinute, map(currentTime.minute, 0, 59, minPWMFrequency, maxPWMFrequency), &this->lastMinute);
    if (format_24H) {
        this->set(this->pinHour, map(currentTime.hour, 0, 23, minPWMFrequency, maxPWMFrequency), &this->lastHour);
    } else {
        int hour = (currentTime.hour-1) % 12;
        this->set(this->pinHour, map(hour, 0, 11, minPWMFrequency, maxPWMFrequency), &this->lastHour);
    }
}

void GBClockController::turnOff() {

    this->set(this->pinSecond, minPWMFrequency, &this->lastSecond);
    this->set(this->pinMinute, minPWMFrequency, &this->lastMinute);
    this->set(this->pinHour, minPWMFrequency, &this->lastHour);
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
    for (int i=minPWMFrequency; i < maxPWMFrequency; ++i) {

        this->set(this->pinSecond, i, &this->lastSecond);
        this->set(this->pinMinute, i, &this->lastMinute);
        this->set(this->pinHour, i, &this->lastHour);
        delay(max((millisec/2) / maxPWMFrequency, 1));
    }
    for (int i=maxPWMFrequency; i >= minPWMFrequency; --i) {
        this->set(this->pinSecond, i, &this->lastSecond);
        this->set(this->pinMinute, i, &this->lastMinute);
        this->set(this->pinHour, i, &this->lastHour);
        delay(max((millisec/2) / maxPWMFrequency, 1));
    }
    this->turnOff();
    delay(100);
    Serial.println("End callibrate");
}