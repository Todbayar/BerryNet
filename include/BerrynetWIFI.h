#ifndef BERRYNETWIFI_H
#define BERRYNETWIFI_H

#include <Arduino.h>
#include <WiFi.h>

class BerrynetWIFI :  {
    private:
        char* ssid, pass;
    public:
        BerrynetWIFI(char* _ssid, char* _pass);
        IPAddress Connect();
        IPAddress Reconnect();

        static IPAddress IP_LOCAL_WIFI;
};

#endif