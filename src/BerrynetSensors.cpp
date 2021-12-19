#include <ArduinoJson.h>
#include <Wire.h>
#include "BerrynetSensors.hpp"
#include "BerrynetVariables.hpp"

Sensors::Sensors(uint16_t _addressLight, uint16_t _pinEnv, uint16_t _pinSoilMoist, uint16_t _pinSoilTemp){
    Sensors::addressLight = _addressLight;
    Sensors::pinEnv = _pinEnv;
    Sensors::pinSoilMoist = _pinSoilMoist;
    Sensors::pinSoilTemp = _pinSoilTemp;
}

Sensors::ModelSensors Read(){
    Wire.begin();   //I2C
    
    //BH1750 light intensity sensor
    BH1750 meterLight(Sensors::addressLight);
    meterLight.begin(BH1750::CONTINUOUS_HIGH_RES_MODE);

    //DHT11 environment humidity and temperature sensor
    DHTesp meterEnvironment;                          
    meterEnvironment.setup(Sensors::pinEnv, DHTesp::DHT11);

    //IC555 gravity soil moisture sensor v2.0
    GravitySoilMoistureSensor meterSoilMoist;      
    meterSoilMoist.Setup(Sensors::pinSoilMoist);

    //Dallas DS18B20 soil temperature sensor
    DS18B20 meterSoilTemp(Sensors::pinSoilTemp);

    // if(Sensors::meterLight.measurementReady()){} 
    // if(Sensors::meterEnvironment.getStatus() == DHTesp::ERROR_NONE){}

    Sensors::ModelSensors modSensors;
    modSensors.light = meterLight.readLightLevel();
    modSensors.environment.humidity = meterEnvironment.getHumidity();
    modSensors.environment.temperature = meterEnvironment.getTemperature();
    modSensors.soil.moisture = meterSoilMoist.Read();
    modSensors.soil.temperature = meterSoilTemp.getTempC();

    return modSensors;
}