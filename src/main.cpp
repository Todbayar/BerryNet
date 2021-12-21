#include <Arduino.h>
#include <U8g2lib.h>    //oled 0.96" 128x64 noname i2c

// #include "freertos/FreeRTOS.h"
// #include "freertos/task.h"
// #include "freertos/event_groups.h"

#include "BerrynetVariables.h"
#include "BerrynetSensors.h"
#include "BerrynetWIFI.h"

#include <PubSubClient.h>
#include <WiFi.h>

U8G2_SSD1306_128X64_NONAME_F_HW_I2C oled(U8G2_R0, U8X8_PIN_NONE);

BerrynetSensors cSensors(SENSOR_ADDRESS_BH1750, SENSOR_PIN_D_DHT, SENSOR_PIN_A_SOIL_MOIST, SENSOR_PIN_D_SOIL_TEMP);
BerrynetWIFI cWifi(WIFI_SSID, WIFI_PASS);
String VAR_GLOBAL_STR_STATUS;

void displayTask(void *param){
    for(;;){
        BerrynetSensors::ModelSensors varSensors = cSensors.Read();

        oled.clearBuffer();
        
        oled.setCursor(0, 15);
        oled.print("Төлөв:");
        oled.print(*((String*)param));
        
        oled.setCursor(0, 30);
        oled.print("Гэрэл:");
        oled.print(varSensors.light);
        oled.print(" lx");

        oled.setCursor(0, 45);
        oled.print("Орчин:");
        oled.print((uint16_t)varSensors.environment.humidity);
        oled.print("% ");
        oled.print((uint16_t)varSensors.environment.temperature);
        oled.print("c");

        oled.setCursor(0, 60);
        oled.print("Хөрс:");
        oled.print(varSensors.soil.moisture);
        oled.print(" ");
        oled.print((uint16_t)varSensors.soil.temperature);
        oled.print("c");

        oled.sendBuffer();
    }

    vTaskDelete(NULL);
}

void WiFiEventFail(WiFiEvent_t event, WiFiEventInfo_t info){
    VAR_GLOBAL_STR_STATUS = "WiFi Fail";
}

void WiFiEventSuccess(WiFiEvent_t event, WiFiEventInfo_t info){
    VAR_GLOBAL_STR_STATUS = "WiFi OK";
}

void setup() {
    oled.begin();
    oled.enableUTF8Print();
    //oled.setFont(u8g2_font_4x6_t_cyrillic);
    oled.setFont(u8g2_font_unifont_t_cyrillic);
    oled.setFontDirection(0);

    xTaskCreate(displayTask, "displayTask", 10000, (void*)&VAR_GLOBAL_STR_STATUS, 1, NULL);
    
    WiFi.onEvent(WiFiEventSuccess, SYSTEM_EVENT_STA_CONNECTED);
    WiFi.onEvent(WiFiEventFail, SYSTEM_EVENT_STA_DISCONNECTED);
    //VAR_GLOBAL_STR_STATUS = cWifi.Connect().toString();
    btStop();
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASS);
    uint16_t counter = 0;
    while (WiFi.status() != WL_CONNECTED){
        VAR_GLOBAL_STR_STATUS = "STAT:" + (String)wl_status_t(WiFi.status());
        delay(500);
        if(counter > 120){
            VAR_GLOBAL_STR_STATUS = "RECONNECT...";
            WiFi.reconnect();
            break;
        }
        counter++;
    };
    VAR_GLOBAL_STR_STATUS = WiFi.localIP().toString();
}

void loop() {
    //VAR_GLOBAL_STR_STATUS = cWifi.Reconnect().toString();
}