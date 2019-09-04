#ifndef GBClockController_h
#define GBClockController_h


#include <GBTime.h>
#include <stdint.h>

class GBClockController {
    public:
        GBClockController(uint8_t pinSecond, uint8_t pinMinute, uint8_t pinHour);
        ~GBClockController();
        void updateTime(GBDateTime currentTime);
        void turnOff();
        void callibrate(int millisec);

    private:
        uint8_t pinSecond;
        uint8_t pinMinute;
        uint8_t pinHour;

        int16_t lastSecond;
        int16_t lastMinute;
        int16_t lastHour;

        void set(uint8_t pin, int16_t value, int16_t* lastValue);
};

#endif