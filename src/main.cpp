#include <Arduino.h>
#include <U8g2lib.h>    //oled 0.96" 128x64 noname i2c

#include "BerrynetVariables.h"
#include "BerrynetSensors.h"

#include <PubSubClient.h>
#include <WiFi.h>

U8G2_SSD1306_128X64_NONAME_F_HW_I2C oled(U8G2_R0, U8X8_PIN_NONE);
BerrynetSensors cSensors(SENSOR_ADDRESS_BH1750, SENSOR_PIN_D_DHT, SENSOR_PIN_A_SOIL_MOIST, SENSOR_PIN_D_SOIL_TEMP);
WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

#define U8LOG_WIDTH 20
#define U8LOG_HEIGHT 8
uint8_t u8log_buffer[U8LOG_WIDTH*U8LOG_HEIGHT];
U8G2LOG u8g2log;

String VAR_GLOBAL_STR_STATUS;
BerrynetSensors::ModelSensors varSensors;

void mqttCallback(char *topic, byte *payload, uint16_t length){
    Serial.print("MQTT:");
    Serial.println(topic);
    oled.setFont(u8g2_font_5x7_tr);	// set the font for the terminal window
    u8g2log.begin(oled, U8LOG_WIDTH, U8LOG_HEIGHT, u8log_buffer);
    u8g2log.setLineHeightOffset(0);	// set extra space between lines in pixel, this can be negative
    u8g2log.setRedrawMode(1);

    //VAR_GLOBAL_STR_STATUS = "MQTT:";
    for(int i=0; i<length; i++){
        Serial.print((char)payload[i]);
        u8g2log.print((char)payload[i]);
    }
    delay(5000);
}

void displayTask(void *param){
    for(;;){
        varSensors = cSensors.Read();

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
    WiFi.disconnect();
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
    delay(1000);

    mqttClient.setServer(MQTT_SERVER, MQTT_PORT);
    mqttClient.setCallback(mqttCallback);
    if(mqttClient.connect(MQTT_CLIENT_ID, MQTT_USERNAME, MQTT_PASSWORD)){
        VAR_GLOBAL_STR_STATUS = "MQTT OK";
    }
    else {
        VAR_GLOBAL_STR_STATUS = "MQTT AUTH?";
    }
}

void setup() {
    Serial.begin(115200);

    oled.begin();
    oled.enableUTF8Print();
    oled.setFont(u8g2_font_unifont_t_cyrillic);
    oled.setFontDirection(0);

    xTaskCreate(displayTask, "displayTask", 10000, (void*)&VAR_GLOBAL_STR_STATUS, 1, NULL);
    
    btStop();
    WiFi.onEvent(WiFiEventSuccess, SYSTEM_EVENT_STA_CONNECTED);
    WiFi.onEvent(WiFiEventFail, SYSTEM_EVENT_STA_DISCONNECTED);
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASS);
}

uint16_t mqtt_counter = WIFI_TIMEOUT/2;
void loop() {
    if(WiFi.status() == WL_CONNECTED){
        switch(mqttClient.state()){
            case MQTT_CONNECTION_TIMEOUT:
                VAR_GLOBAL_STR_STATUS = "MQTT TIMEOUT";
                break;
            case MQTT_CONNECTED:
                VAR_GLOBAL_STR_STATUS = "MQTT OK";
                break;
        }
        delay(1000);
        if(mqttClient.connected()){
            VAR_GLOBAL_STR_STATUS = "MQTT:" + mqtt_counter;
            if(mqtt_counter > 0){
                char mqttData[50];
                snprintf(mqttData, 50, "Numeric #%1d", varSensors.light);
                mqttClient.publish("channels/1613491/publish/fields/field1", mqttData);
                mqtt_counter = WIFI_TIMEOUT/2;
            }
            mqtt_counter--;
        }
        else {
            VAR_GLOBAL_STR_STATUS = "MQTT FAIL";
        }
        mqttClient.loop();
        delay(1000);
        VAR_GLOBAL_STR_STATUS = WiFi.localIP().toString();
        delay(1000);
    }
    //if(mqttClient.subscribe(MQTT_ID_READ)){
    //             Serial.println("Mqtt Subscribed");
    //         }
}