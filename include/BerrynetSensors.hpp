#include <BH1750.h>
#include <DS18B20.h>
#include "DHTesp.h"
#include "gravity_soil_moisture_sensor.h"
#include "BerrynetModels.hpp"
#include "BerrynetVariables.hpp"

class Sensors {
    private:
        BH1750 meterLight(int address);             //light sensor
        DHTesp meterEnvironment;                    //environment humi & temp
        GravitySoilMoistureSensor meterSoilMoist;   //soil moist
        DS18B20 meterSoilTemp(int pin);             //soil temp

    public:
        Sensors(
            int _addressLight = SENSOR_ADDRESS_BH1750, 
            int _pinEnv = SENSOR_PIN_D_DHT, 
            int _pinSoilMoist = SENSOR_PIN_A_SOIL_MOIST, 
            int _pinSoilTemp = SENSOR_PIN_D_SOIL_TEMP
        );

        void setAddressLight(int _address = SENSOR_ADDRESS_BH1750);
        
        void setPinEnv(int _pin = SENSOR_PIN_D_DHT);
        
        void setPinSoilMoist(int _pin = SENSOR_PIN_A_SOIL_MOIST);
        
        void setPinSoilTemp(int _pin = SENSOR_PIN_D_SOIL_TEMP); 
        
        ModelSensors Read();

        int addressLight, pinEnv, pinSoilMoist, pinSoilTemp; 
};