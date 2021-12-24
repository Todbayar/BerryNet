#ifndef BERRYNETPLANTER_H
#define BERRYNETPLANTER_H

#include <Arduino.h>

class BerrynetPlanter {
    private:
        uint16_t pinLight, pinHeater, pinWaterPump, pinWaterMeter;
        ulong pumpWaitMinute, pumpLastTime;

    public:
        typedef enum {
            ON,
            OFF
        } turn_t;

        BerrynetPlanter(uint16_t _pinLight, uint16_t _pinHeater, uint16_t _pinWaterPump, uint16_t _pinWaterMeter);
        bool Light(turn_t _turn);
        bool Heater(turn_t _turn);
        ulong Pump(ulong _waitMinute, ulong _runSecond);
};

#endif