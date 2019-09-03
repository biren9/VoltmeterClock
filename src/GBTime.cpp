#include <GBTime.h>
#include <NTPClient.h>
#include <Timezone.h>
#include <WiFiUdp.h>
#include <DS1307RTC.h>

#define RTCSyncPeriod 3600000 // 1 Hour

const char* GBTime::daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

GBTime::GBTime() {
    TimeChangeRule CEST = {"CEST", Last, Sun, Mar, 2, 120};     // Central European Summer Time
    TimeChangeRule CET = {"CET ", Last, Sun, Oct, 3, 60};       // Central European Standard Time
    this->ce = new Timezone(CEST, CET);
    this->ntpUDP = new WiFiUDP();
    this->timeClient = new NTPClient(*this->ntpUDP, "europe.pool.ntp.org");
    this->timeClient->setUpdateInterval(60000);
    this->lastUpdateRTC = millis();
    this->forceUpdate = true;
}

GBTime::~GBTime() {
    this->end();
    delete this->ce;
    delete this->ntpUDP;
    delete this->timeClient;
}

void GBTime::begin() {
    this->timeClient->begin();
}

void GBTime::end() {
    this->timeClient->end();
}

GBDateTime GBTime::currentTime() {
    bool success = this->timeClient->update();
    
    if (success && (millis() - this->lastUpdateRTC >= RTCSyncPeriod || millis() - this->lastUpdateRTC < 0) || this->forceUpdate ) {
        Serial.println("Sync RTC with NTP");
        RTC.set(this->timeClient->getEpochTime());
        this->lastUpdateRTC = millis();
        this->forceUpdate = false;
    }

    time_t convertedTime = this->ce->toLocal(RTC.get());

    GBDateTime dt = {
        second(convertedTime),
        minute(convertedTime),
        hour(convertedTime),
        day(convertedTime),
        month(convertedTime),
        year(convertedTime),
        weekday(convertedTime) -1
    };
    return dt;
}