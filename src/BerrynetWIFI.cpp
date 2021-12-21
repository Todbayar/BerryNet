#include <Arduino.h>
#include <WiFi.h>
#include "BerrynetWIFI.h"

BerrynetWIFI::BerrynetWIFI(const char* _ssid, const char* _pass){
    this->ssid = &_ssid;
    this->pass = &_pass;
}

IPAddress BerrynetWIFI::Connect(){
    //btStop();
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, pass);
    uint16_t counter = 0;
    while (WiFi.status() != WL_CONNECTED){
        delay(500);
        if(counter > 60){
            break;
        }
        counter++;
    };
    this->IP_LCOAL_WIFI = WiFi.localIP();
    return this->IP_LCOAL_WIFI;
}

IPAddress BerrynetWIFI::Reconnect(){
    uint16_t counter = 0;
    //btStop();
    while (WiFi.status() != WL_CONNECTED){
        WiFi.disconnect();
        delay(100);
        WiFi.reconnect();
        delay(400);
        if(counter > 60){
            ESP.restart();
        }
        counter++;
    };
    this->IP_LCOAL_WIFI = WiFi.localIP();
    return this->IP_LCOAL_WIFI;
}