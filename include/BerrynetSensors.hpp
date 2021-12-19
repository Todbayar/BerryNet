#include <BH1750.h>
#include <DS18B20.h>
#include "DHTesp.h"
#include "gravity_soil_moisture_sensor.h"

class Sensors {
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

        Sensors(uint16_t _addressLight, uint16_t _pinEnv, uint16_t _pinSoilMoist, uint16_t _pinSoilTemp);
        
        ModelSensors Read();

        static int addressLight, pinEnv, pinSoilMoist, pinSoilTemp; 
};