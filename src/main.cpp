#include <Arduino.h>
#include <U8g2lib.h>    //oled 0.96" 128x64 noname i2c

#include "BerrynetVariables.h"
#include "BerrynetSensors.h"

#include <PubSubClient.h>
#include <WiFi.h>

U8G2_SSD1306_128X64_NONAME_F_HW_I2C oled(U8G2_R0, U8X8_PIN_NONE);
BerrynetSensors cSensors(SENSOR_ADDRESS_BH1750, SENSOR_PIN_D_DHT, SENSOR_PIN_A_SOIL_MOIST, SENSOR_PIN_D_SOIL_TEMP);
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
        oled.print("LX");

        oled.setCursor(0, 45);
        oled.print("Орчин:");
        oled.print((uint16_t)varSensors.environment.humidity);
        oled.print("% ");
        oled.print((uint16_t)varSensors.environment.temperature);
        oled.print("C");

        oled.setCursor(0, 60);
        oled.print("Хөрс:");
        oled.print(varSensors.soil.moisture);
        oled.print(" ");
        oled.print((uint16_t)varSensors.soil.temperature);
        oled.print("C");

        oled.sendBuffer();
    }

    vTaskDelete(NULL);
}

uint16_t wifi_try = 0;
void WiFiEventFail(WiFiEvent_t event, WiFiEventInfo_t info){
    VAR_GLOBAL_STR_STATUS = "WiFi Fail";
    //WiFi.disconnect();
    WiFi.reconnect();
    uint16_t dottimer = 1;
    while(WiFi.status() != WL_CONNECTED){
        if(wifi_try > WIFI_TIMEOUT){
            VAR_GLOBAL_STR_STATUS = "Restart...";
            delay(1000);
            ESP.restart();
        }
        delay(1000);
        switch(dottimer){
            case 1:
                VAR_GLOBAL_STR_STATUS = "WiFi";
                break;
            case 2:
                VAR_GLOBAL_STR_STATUS = "WiFi.";
                break;
            case 3:
                VAR_GLOBAL_STR_STATUS = "WiFi..";
                break;
            case 4:
                VAR_GLOBAL_STR_STATUS = "WiFi...";
                dottimer = 0;
                break;
        }
        dottimer++;
        wifi_try++;
    };
}

void WiFiEventSuccess(WiFiEvent_t event, WiFiEventInfo_t info){
    VAR_GLOBAL_STR_STATUS = "WiFi OK";
    delay(1000);
    VAR_GLOBAL_STR_STATUS = WiFi.localIP().toString();

}

void setup() {
    oled.begin();
    oled.enableUTF8Print();
    oled.setFont(u8g2_font_unifont_t_cyrillic);
    oled.setFontDirection(0);

    WiFi.onEvent(WiFiEventSuccess, SYSTEM_EVENT_STA_CONNECTED);
    WiFi.onEvent(WiFiEventFail, SYSTEM_EVENT_STA_DISCONNECTED);
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASS);

    xTaskCreate(displayTask, "displayTask", 10000, (void*)&VAR_GLOBAL_STR_STATUS, 1, NULL);
}

void loop() {
}