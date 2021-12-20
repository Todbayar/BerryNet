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

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);
long last_time;
uint16_t dataToPublish[5];
uint16_t fieldToPublish[5] = {1,1,0,0,0};

void mqttCallback(char *topic, byte *payload, uint16_t length){
    Serial.print("Callback - Message:");
    for(int i=0; i<length; i++){
        Serial.print((char)payload[i]);
    }
    Serial.println();
}

void mqttReconnect(){
    Serial.println("Connecting to MQTT broker...");
    while(!mqttClient.connected()){
        Serial.println("Connecting to MQTT broker...");
        if(mqttClient.connect(MQTT_CLIENT_ID)){
            Serial.println("Connected.");
        }
    }
}

void setup() {
    Serial.begin(115200);
    while(!Serial);
    oled.begin();
    oled.enableUTF8Print();
    //oled.setFont(u8g2_font_4x6_t_cyrillic);
    oled.setFont(u8g2_font_unifont_t_cyrillic);
    oled.setFontDirection(0);

    cWifi.Connect();
    mqttClient.setServer(MQTT_SERVER, 8883);
    mqttClient.setCallback(mqttCallback);
}

void loop() {
    cWifi.Reconnect();

    if(!mqttClient.connected()){
        if(mqttClient.connect(MQTT_CLIENT_ID, MQTT_USERNAME, MQTT_PASSWORD)){
            if(mqttClient.subscribe(MQTT_ID_READ)){
                Serial.println("Mqtt Subscribed");
            }
        }
        else {
            Serial.println(mqttClient.state());
            delay(5000);
        }
    }
    mqttClient.loop();

    BerrynetSensors::ModelSensors varSensors = cSensors.Read();

    oled.clearBuffer();
    
    oled.setCursor(0, 15);
    oled.print(cWifi.ipLocalWifi);
    
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
    
    long now = millis(); 
    if(now - last_time > 60000){
        dataToPublish[0] = varSensors.light;
        //mqttClient.publish(MQTT_ID_WRITE, dataToPublish, fieldToPublish);
        last_time = now;
    }
    delay(1);
}