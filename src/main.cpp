#include <Arduino.h>
#include <U8g2lib.h>    //oled 0.96" 128x64 noname i2c
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

#include "BerrynetVariables.h"
#include "BerrynetSensors.h"
#include "BerrynetModels.h"
#include "BerrynetPlantModels.h"
#include "BerrynetPlanter.h"

//GLOBAL CLASSES
U8G2_SSD1306_128X64_NONAME_F_HW_I2C oled(U8G2_R0, U8X8_PIN_NONE);
BerrynetSensors cSensors(SENSOR_ADDRESS_BH1750, SENSOR_PIN_D_DHT, SENSOR_PIN_A_SOIL_MOIST, SENSOR_PIN_D_SOIL_TEMP);
BerrynetPlanter cPlanter(PLANTER_PIN_LIGHT, PLANTER_PIN_HEATER, PLANTER_PIN_PUMP_MOTOR, PLANTER_PIN_PUMP_METER);

//GLOBAL VARIABLES
String VAR_GLOBAL_STR_STATUS;
ModelSensors varSensors;
ModelBerry varBerry;
uint16_t time_started = millis();

//FUNCTIONS
void displayOled(){
    oled.clearBuffer();

    oled.setCursor(0, 15);
    oled.print("Төлөв:");
    oled.print(VAR_GLOBAL_STR_STATUS);
    
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
    oled.print("% ");
    oled.print((uint16_t)varSensors.soil.temperature);
    oled.print("C");

    oled.sendBuffer();
}

void sensorsTask(void *param){
    for(;;){
        varSensors = cSensors.Read();
        displayOled();

        if(varSensors.light < varBerry.light.opt){
            cPlanter.Light(cPlanter.ON);
        }
        else {
            cPlanter.Light(cPlanter.OFF);
        }

        if(varSensors.soil.temperature < varBerry.soil.temp.max){
            cPlanter.Heater(cPlanter.ON);
        }
        else {
            cPlanter.Heater(cPlanter.OFF);
        }

        //Need to buy 12v water pump and single relay module
        if(varSensors.soil.moisture < varBerry.soil.moist.min){
            cPlanter.Pump(PLANTER_TIME_PUMP_WAIT, PLANTER_TIME_PUMP_RUN);
        }
    }
    vTaskDelete(NULL);
}

void postTask(void *param){
    for(;;){
        if(WiFi.status() == WL_CONNECTED && millis() - time_started > HTTP_TIMEUP){
            //SEND SENSORS' DATA TO IoT SERVER
            VAR_GLOBAL_STR_STATUS = "POST...";
            ModelSensors pSensors = *((ModelSensors*)param);
            StaticJsonDocument<96> jsonData;    //Calculated by "ArduinoJSON" (web site)
            jsonData["light"] = pSensors.light;
            jsonData["env_humi"] = pSensors.environment.humidity;
            jsonData["env_temp"] = pSensors.environment.temperature;
            jsonData["soil_moist"] = pSensors.soil.moisture;
            jsonData["soil_temp"] = pSensors.soil.temperature;
            String jsonStr;
            serializeJson(jsonData, jsonStr);
            WiFiClient wifiClient;
            HTTPClient httpClient;
            httpClient.begin(wifiClient, HTTP_URL_POST);
            httpClient.addHeader("Content-Type", "application/json");
            httpClient.addHeader("Accept", "text/plain");
            httpClient.addHeader("Authorization", strcpy("Bearer ", HTTP_API_KEY)); //NEEDS TO BE FIXED!!!
            uint16_t httpResponseCode = httpClient.POST(jsonStr);
            if(httpResponseCode == HTTP_CODE_ACCEPTED || httpResponseCode == HTTP_CODE_OK){
                VAR_GLOBAL_STR_STATUS = "POST OK";
            }
            else if(httpResponseCode == HTTP_CODE_UNAUTHORIZED){
                VAR_GLOBAL_STR_STATUS = "POST AUTH!";
            }
            else {
                VAR_GLOBAL_STR_STATUS = "POST FAIL";
            }
            time_started = millis();
        }
    }
    vTaskDelete(NULL);
}

uint16_t wifi_try = millis();
void WiFiEventFail(WiFiEvent_t event, WiFiEventInfo_t info){
    VAR_GLOBAL_STR_STATUS = "WiFi!";
    WiFi.disconnect();
    WiFi.reconnect();
    uint16_t dottimer = 1;
    while(WiFi.status() != WL_CONNECTED){
        if(millis() - wifi_try > WIFI_TIMEOUT){
            VAR_GLOBAL_STR_STATUS = "WiFi fail";
            //delay(1000);
            //ESP.restart();
            break;
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
        wifi_try = millis();
    };
}

void WiFiEventSuccess(WiFiEvent_t event, WiFiEventInfo_t info){
    VAR_GLOBAL_STR_STATUS = WiFi.localIP().toString();
    xTaskCreate(postTask, "postTask", 7000, &varSensors, 2, NULL);
}

void setup() {
    //Serial.begin(115200);
    pinMode(2, OUTPUT);
    digitalWrite(2, LOW);

    oled.begin();
    oled.enableUTF8Print();
    oled.setFont(u8g2_font_unifont_t_cyrillic);
    oled.setFontDirection(0);

    xTaskCreate(sensorsTask, "sensorsTask", 10000, NULL, 1, NULL);

    btStop();
    WiFi.onEvent(WiFiEventSuccess, SYSTEM_EVENT_STA_CONNECTED);
    WiFi.onEvent(WiFiEventFail, SYSTEM_EVENT_STA_DISCONNECTED);
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASS);
}

void loop() {
    //Avoid to use it if freeRTOS's been used
}