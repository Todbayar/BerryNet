#include <Arduino.h>
#include <WiFi.h>
#include "BerrynetWIFI.h"

BerrynetWIFI::BerrynetWIFI(String _ssid, String _pass){
    ssid = _ssid;
    pass = _pass;
    // btStop();
    // WiFi.mode(WIFI_STA);
    // WiFi.disconnect();
    // WiFi.begin(ssid.c_str(), pass.c_str());
}

IPAddress BerrynetWIFI::Connect(){
    while (WiFi.status() != WL_CONNECTED){
        WiFi.disconnect();
        delay(500);
        WiFi.begin(ssid.c_str(), pass.c_str());
    };
    IPAddress vLocalIP = WiFi.localIP();
    //log_i("IP_ADDRESS:%s", vLocalIP.toString())
    return vLocalIP;
}