#include <Arduino.h>
#include <U8g2lib.h>    //oled 0.96" 128x64 noname i2c
#include "BerrynetSensors.h"
#include "BerrynetVariables.h"

U8G2_SSD1306_128X64_NONAME_F_HW_I2C oled(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

Sensors fSensors(SENSOR_ADDRESS_BH1750, SENSOR_PIN_D_DHT, SENSOR_PIN_A_SOIL_MOIST, SENSOR_PIN_D_SOIL_TEMP);

void setup() {
    Serial.begin(115200);
    while(!Serial);

    oled.begin();
    oled.enableUTF8Print();
    //oled.setFont(u8g2_font_4x6_t_cyrillic);
    oled.setFont(u8g2_font_unifont_t_cyrillic);
    oled.setFontDirection(0);
}

void loop() {
    Sensors::ModelSensors varSensors = fSensors.ReadSensors();
    oled.clearBuffer();

    oled.setCursor(0, 15);
    oled.print(F("BerryNet"));
    
    oled.setCursor(0, 30);
    oled.print(F("Гэрэл:"));
    oled.print(varSensors.light);
    
    oled.setCursor(0, 45);
    oled.print(F("Орчин:"));
    oled.print((int16_t)varSensors.environment.humidity);
    oled.print(F("%, "));
    oled.print((int16_t)varSensors.environment.temperature);
    oled.print(F("c"));
    
    oled.setCursor(0, 60);
    oled.print(F("Хөрс:"));
    oled.print(varSensors.soil.moisture);
    oled.print(F(", "));
    oled.print((int16_t)varSensors.soil.temperature);
    oled.print(F("c"));
    
    oled.sendBuffer();
}