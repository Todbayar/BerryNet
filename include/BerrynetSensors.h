#ifndef BERRYNETSENSORS_H
#define BERRYNETSENSORS_H

class BerrynetSensors {
    private:
        uint16_t addressLight, pinEnv, pinSoilMoist, pinSoilTemp;
        
    public:
        struct ModelSensors {
            struct Environment {
                float humidity = 0;
                float temperature = 0;
            } environment;
            struct Soil {
                uint16_t moisture = 0;
                float temperature = 0;  
            } soil;
            uint16_t light = 0;
        };
        
        BerrynetSensors(uint16_t _addressLight, uint16_t _pinEnv, uint16_t _pinSoilMoist, uint16_t _pinSoilTemp);

        ModelSensors Read();
};

#endif