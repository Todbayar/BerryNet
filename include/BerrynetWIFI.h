#ifndef BERRYNETWIFI_H
#define BERRYNETWIFI_H

class BerrynetWIFI {
    private:
        String ssid, pass;
    public:
        BerrynetWIFI(String _ssid, String _pass);
        IPAddress Connect();
        IPAddress Reconnect();
        IPAddress ipLocalWifi;
};

#endif