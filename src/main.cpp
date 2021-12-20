#include <Arduino.h>
#include <U8g2lib.h>    //oled 0.96" 128x64 noname i2c
#include <WiFi.h>

// #include "freertos/FreeRTOS.h"
// #include "freertos/task.h"
// #include "freertos/event_groups.h"

#include "BerrynetVariables.h"
#include "BerrynetSensors.h"
#include "BerrynetWIFI.h"


U8G2_SSD1306_128X64_NONAME_F_HW_I2C oled(U8G2_R0, U8X8_PIN_NONE);

BerrynetSensors cSensors(SENSOR_ADDRESS_BH1750, SENSOR_PIN_D_DHT, SENSOR_PIN_A_SOIL_MOIST, SENSOR_PIN_D_SOIL_TEMP);
//BerrynetWIFI cWifi(WIFI_SSID, WIFI_PASS);

//IPAddress wifiIPAddress;

void setup() {
    Serial.begin(115200);
    while(!Serial);
    oled.begin();
    oled.enableUTF8Print();
    //oled.setFont(u8g2_font_4x6_t_cyrillic);
    oled.setFont(u8g2_font_unifont_t_cyrillic);
    oled.setFontDirection(0);

    //wifiIPAddress = cWifi.Connect();

    btStop();
    WiFi.mode(WIFI_STA);
    Serial.println(WIFI_SSID);
    Serial.println(WIFI_PASS);
    WiFi.begin(WIFI_SSID, WIFI_PASS);

    while (WiFi.status() != WL_CONNECTED){
        delay(500);
        Serial.print('.');
    };
}

void loop() {
    BerrynetSensors::ModelSensors varSensors = cSensors.Read();

    oled.clearBuffer();
    
    oled.setCursor(0, 15);
    oled.print(WiFi.localIP());
    
    oled.setCursor(0, 30);
    oled.print("Light:");
    oled.print(varSensors.light);
    oled.print(" lx");

    oled.setCursor(0, 45);
    oled.print("Env:");
    oled.print((uint16_t)varSensors.environment.humidity);
    oled.print("% ");
    oled.print((uint16_t)varSensors.environment.temperature);
    oled.print("c");

    oled.setCursor(0, 60);
    oled.print("Soil:");
    oled.print(varSensors.soil.moisture);
    oled.print(" ");
    oled.print(varSensors.soil.temperature);
    oled.print("c");

    oled.sendBuffer();
}