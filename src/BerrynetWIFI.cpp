#include <Arduino.h>
#include <WiFi.h>
#include "BerrynetWIFI.h"

BerrynetWIFI::BerrynetWIFI(String _ssid, String _pass){
    ssid = _ssid;
    pass = _pass;
}

IPAddress BerrynetWIFI::Connect(){
    log_i("Connecting to WiFi");
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid.c_str(), pass.c_str());
    uint16_t counter = 0;
    while (WiFi.status() != WL_CONNECTED){
        log_i(".");
        delay(500);
        if(counter > 120){
            break;
        }
        counter++;
    };
    ipLocalWifi = WiFi.localIP();
    log_i("IP_ADDRESS:%s", ipLocalWifi.toString())
    return ipLocalWifi;
}

IPAddress BerrynetWIFI::Reconnect(){
    log_i("Reconnecting to WiFi");
    uint16_t counter = 0;
    while (WiFi.status() != WL_CONNECTED){
        log_i(".");
        WiFi.disconnect();
        delay(100);
        WiFi.reconnect();
        delay(400);
        if(counter > 120){
            ESP.restart();
        }
        counter++;
    };
    ipLocalWifi = WiFi.localIP();
    log_i("WiFi Reconnected, IP_ADDRESS:%s", ipLocalWifi.toString())
    return ipLocalWifi;
}