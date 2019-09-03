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
};

#endif