#include <GBTime.h>
#include <NTPClient.h>
#include <Timezone.h>
#include <WiFiUdp.h>
#include <DS1307RTC.h>
#include <WiFiManager.h>

#define RTCSyncPeriod 60*60*1000 // 1 Hour
#define WIFI_ConnectingTimeout 30*1000 // 30 Seconds
#define WIFI_SSID "VoltmeterClock"
#define WIFI_PASSWORD "11111111"

const char* GBTime::daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

GBTime::GBTime(WiFiManager* manager) {
    TimeChangeRule CEST = {"CEST", Last, Sun, Mar, 2, 120};     // Central European Summer Time
    TimeChangeRule CET = {"CET ", Last, Sun, Oct, 3, 60};       // Central European Standard Time
    this->manager = manager;
    this->ce = new Timezone(CEST, CET);
    this->ntpUDP = new WiFiUDP();
    this->timeClient = new NTPClient(*this->ntpUDP, "europe.pool.ntp.org");
    this->timeClient->setUpdateInterval(60000);
    this->lastUpdateRTC = millis();
    this->forceUpdate = true;
    this->wifiConnectTime = 0;
}

GBTime::~GBTime() {
    this->end();
    delete this->ce;
    delete this->ntpUDP;
    delete this->timeClient;
    delete this->manager;
}

void GBTime::begin() {
    this->timeClient->begin();
    this->manager->autoConnect(WIFI_SSID, WIFI_PASSWORD);
}

void GBTime::end() {
    this->timeClient->end();
    WiFi.mode(WIFI_OFF);
    WiFi.forceSleepBegin();
}

GBDateTime GBTime::currentTime() {
    if (millis() - this->lastUpdateRTC >= RTCSyncPeriod || millis() - this->lastUpdateRTC < 0 || this->forceUpdate ) {
        if (WiFi.status() == WL_CONNECTED) {
            bool success = this->timeClient->update();
            if (success) {
                Serial.println("Sync RTC with NTP");
                RTC.set(this->timeClient->getEpochTime());
                this->lastUpdateRTC = millis();
                this->forceUpdate = false;
            }
            WiFi.mode(WIFI_OFF);
            WiFi.forceSleepBegin();
            delay(1);
            Serial.println("Send WiFi module to sleep");
        } else if (millis() - this->wifiConnectTime > WIFI_ConnectingTimeout) {
            Serial.println("Trying to reconnect");
            WiFi.forceSleepWake();
            WiFi.begin(WiFi.SSID(), WiFi.psk());
            this->wifiConnectTime = millis();
        } else {
            Serial.print(".");
        }
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