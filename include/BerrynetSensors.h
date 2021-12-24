#ifndef BERRYNETSENSORS_H
#define BERRYNETSENSORS_H

#include "BerrynetModels.h"

class BerrynetSensors {
    private:
        uint16_t addressLight, pinEnv, pinSoilMoist, pinSoilTemp;
        
    public:
        BerrynetSensors(uint16_t _addressLight, uint16_t _pinEnv, uint16_t _pinSoilMoist, uint16_t _pinSoilTemp);
        ModelSensors Read();
};

#endif