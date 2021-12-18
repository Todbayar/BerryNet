#include <ArduinoJson.h>
#include <Wire.h>
#include "BerrynetSensors.hpp"
#include "BerrynetModels.hpp"

ModelSensors modSensors;    //data model for json serialization

// Sensors::Sensors(){
//     Wire.begin();   //I2C

//     //BH1750 light intensity sensor
//     Sensors::meterLight(int Sensors::addressLight = SENSOR_ADDRESS_BH1750);
//     Sensors::meterLight.begin(BH1750::CONTINUOUS_HIGH_RES_MODE);
    
//     //DHT11 environment humidity and temperature sensor
//     Sensors::meterEnvironment.setup(int Sensors::pinEnv = SENSOR_PIN_D_DHT, DHTesp::DHT11);
    
//     //IC555 gravity soil moisture sensor v2.0
//     Sensors::meterSoilMoist.Setup(int Sensors::pinSoilMoist = SENSOR_PIN_A_SOIL_MOIST);
    
//     //Dallas DS18B20 soil temperature sensor
//     Sensors::meterSoilTemp(int Sensors::pinSoilTemp = SENSOR_PIN_D_SOIL_TEMP);
// }

// Sensors::Sensors(int _pinEnv, int _pinSoilMoist, int _pinSoilTemp){
//     Wire.begin();   //I2C
    
//     //BH1750 light intensity sensor
//     Sensors::meterLight(Sensors::addressLight = SENSOR_ADDRESS_BH1750);
//     Sensors::meterLight.begin(BH1750::CONTINUOUS_HIGH_RES_MODE);
    
//     //DHT11 environment humidity and temperature sensor
//     Sensors::meterEnvironment.setup(Sensors::pinEnv = _pinEnv, DHTesp::DHT11);
    
//     //IC555 gravity soil moisture sensor v2.0
//     Sensors::meterSoilMoist.Setup(Sensors::pinSoilMoist = _pinSoilMoist);
    
//     //Dallas DS18B20 soil temperature sensor
//     Sensors::meterSoilTemp(Sensors::pinSoilTemp = _pinSoilTemp);
// }

// Sensors::Sensors(int _addressLight, int _pinEnv, int _pinSoilMoist, int _pinSoilTemp){
//     Wire.begin();   //I2C
    
//     //BH1750 light intensity sensor
//     Sensors::meterLight(Sensors::addressLight = _addressLight);
//     Sensors::meterLight.begin(BH1750::CONTINUOUS_HIGH_RES_MODE);
    
//     //DHT11 environment humidity and temperature sensor
//     Sensors::meterEnvironment.setup(Sensors::pinEnv = _pinEnv, DHTesp::DHT11);
    
//     //IC555 gravity soil moisture sensor v2.0
//     Sensors::meterSoilMoist.Setup(Sensors::pinSoilMoist = _pinSoilMoist);
    
//     //Dallas DS18B20 soil temperature sensor
//     Sensors::meterSoilTemp(Sensors::pinSoilTemp = _pinSoilTemp);
// }

// Sensors::setAddressLight(){
//     Wire.begin();   //I2C
//     //BH1750 light intensity sensor
//     Sensors::meterLight(Sensors::addressLight = SENSOR_ADDRESS_BH1750);
//     Sensors::meterLight.begin(BH1750::CONTINUOUS_HIGH_RES_MODE);
// }

// Sensors::setAddressLight(int _address){
//     Wire.begin();   //I2C
//     //BH1750 light intensity sensor
//     Sensors::meterLight(Sensors::addressLight = _address);
//     Sensors::meterLight.begin(BH1750::CONTINUOUS_HIGH_RES_MODE);
// }

// Sensors::setPinEnv(int pin){

// }

// bnModelSensors Read(){
//     // if (lightMeter.measurementReady()) 
//     //     modSensors.light = lightMeter.readLightLevel() | 0;

//     // if(dht.getStatus() == DHTesp::ERROR_NONE){
//     //     modSensors.environment.humidity = dht.getHumidity() | 0;
//     //     modSensors.environment.temperature = dht.getTemperature() | 0;
//     // }

//     // modSensors.soil.moisture = soilMoist.Read() | 0;
//     // modSensors.soil.temperature = soilTemp.getTempC() | 0f;

//     // return modSensors;
//     return NULL;
// }