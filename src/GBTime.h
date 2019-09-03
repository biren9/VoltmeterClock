#ifndef GBTime_h
#define GBTime_h

#include <time.h>

class WiFiUDP;
class NTPClient;
class GBDateTime;
class Timezone;

class GBTime {
    public:
        static const char* daysOfTheWeek[7][12];

        GBTime();
        ~GBTime();
        void begin();
        void end();
        GBDateTime currentTime();
    
    private:
        WiFiUDP *ntpUDP;
        NTPClient *timeClient;
        Timezone *ce;
        time_t lastUpdateRTC;
        bool forceUpdate;

};

struct GBDateTime {
    int second;
    int minute;
    int hour;

    int day;
    int month;
    int year;

    int dayOfWeek;

    const char* weekDay() {
        return *GBTime::daysOfTheWeek[dayOfWeek];
    }
};

#endif