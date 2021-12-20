#include <ArduinoJson.h>
#include <Wire.h>
#include <BH1750.h>
#include <DS18B20.h>
#include "DHTesp.h"
#include "gravity_soil_moisture_sensor.h"
#include "BerrynetSensors.h"

BerrynetSensors::BerrynetSensors(uint16_t _addressLight, uint16_t _pinEnv, uint16_t _pinSoilMoist, uint16_t _pinSoilTemp){
    addressLight = _addressLight;
    pinEnv = _pinEnv;
    pinSoilMoist = _pinSoilMoist;
    pinSoilTemp = _pinSoilTemp;
}

BerrynetSensors::ModelSensors BerrynetSensors::Read(){
    Wire.begin();   //I2C
    
    //BH1750 light intensity sensor
    BH1750 meterLight(addressLight);
    meterLight.begin(BH1750::CONTINUOUS_HIGH_RES_MODE);

    //DHT11 environment humidity and temperature sensor
    DHTesp meterEnvironment;                          
    meterEnvironment.setup(pinEnv, DHTesp::DHT11);

    //IC555 gravity soil moisture sensor v2.0
    GravitySoilMoistureSensor meterSoilMoist;      
    meterSoilMoist.Setup(pinSoilMoist);

    //Dallas DS18B20 soil temperature sensor
    DS18B20 meterSoilTemp(pinSoilTemp);

    // if(meterLight.measurementReady()){} 
    // if(meterEnvironment.getStatus() == DHTesp::ERROR_NONE){}

    ModelSensors modSensors;
    modSensors.light = meterLight.readLightLevel();
    modSensors.environment.humidity = meterEnvironment.getHumidity();
    modSensors.environment.temperature = meterEnvironment.getTemperature();
    modSensors.soil.moisture = meterSoilMoist.Read();
    modSensors.soil.temperature = meterSoilTemp.getTempC();

    return modSensors;
}