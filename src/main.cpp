#include <Arduino.h>
#include <Wire.h>
#include <BH1750.h>
#include <DS18B20.h>
#include "DHTesp.h"
#include "gravity_soil_moisture_sensor.h"

#define DHTPIN 4
#define SOILPINA 35
#define SOILTEMPPIN 26
#define BH1750_I2C 0x23

BH1750 lightMeter(BH1750_I2C);
DHTesp dht;
GravitySoilMoistureSensor soilMoist;
DS18B20 soilTemp(SOILTEMPPIN);

void setup() {
    Serial.begin(115200);
    while(!Serial);

    Wire.begin();
    dht.setup(DHTPIN, DHTesp::DHT11);
    lightMeter.begin(BH1750::CONTINUOUS_HIGH_RES_MODE);
    soilMoist.Setup(SOILPINA);
    Serial.println(soilTemp.getNumberOfDevices());
}

void loop() {
    if (lightMeter.measurementReady()) {
        float lightVal = lightMeter.readLightLevel();
        Serial.print(F("Light:"));
        Serial.print(lightVal);
        Serial.println(F("lx"));
    }

    if(dht.getStatus() == DHTesp::ERROR_NONE){
        float humiVal = dht.getHumidity();
        float tempVal = dht.getTemperature();

        Serial.print(F("Environment: humi:"));
        Serial.print(humiVal);
        Serial.print(F("%, temp:"));
        Serial.print(tempVal);
        Serial.println(F("c"));
    }

    uint16_t soilMoistVal = soilMoist.Read();
    Serial.print(F("Soil: moist:"));
    Serial.print(soilMoistVal);

    Serial.print(F(", temp:"));
    Serial.print(soilTemp.getTempC());
    Serial.println(F("c"));
}