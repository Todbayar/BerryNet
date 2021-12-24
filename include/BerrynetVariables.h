#ifndef BERRYNETVARIABLES_H
#define BERRYNETVARIABLES_H

#define TITLE "BerryNet"

#define SENSOR_PIN_D_DHT 25          //Orchnii tempperature bolon chiigshil hemjih medregchiin digital hul DHT11
#define SENSOR_ADDRESS_BH1750 0x23  //Gerel medregch BH1750 chipnii i2c hayag
#define SENSOR_PIN_A_SOIL_MOIST 35  //Hursnii chiigshil medregch 555 chipnii analog hul
#define SENSOR_PIN_D_SOIL_TEMP 26   //Hursnii temperature DS18B20 chipnii digital hul 
#define PLANTER_PIN_LIGHT 14
#define PLANTER_PIN_HEATER 27
#define PLANTER_PIN_PUMP_MOTOR 32
#define PLANTER_PIN_PUMP_METER 4
#define PLANTER_TIME_PUMP_RUN 1500      //1.5sec to run water pump motor until fill with water and depending on water flow meter and water hose length
#define PLANTER_TIME_PUMP_WAIT 300000   //5min for waiting soil moisture raises up or not 5*60*1000

#define WIFI_SSID "Univision_3843"
#define WIFI_PASS "db0fb09b8950"
#define WIFI_TIMEOUT 60000
// #define WIFI_SSID "AndroiAPBADD"
// #define WIFI_PASS "ahnd4523"

// #define MQTT_SERVER "mqtt3.thingspeak.com"
//#define MQTT_PORT 8883
// #define MQTT_PORT 1883
// #define MQTT_ID_WRITE "1JUWJT91TAF6KJC9"
// #define MQTT_ID_READ "OFAP1S6YUB6ZA04D"
// #define MQTT_CHANNELID 1613491
// #define MQTT_USERNAME "LQIbJhgwDR8oCDgSPQo4MT0"
// #define MQTT_CLIENT_ID "LQIbJhgwDR8oCDgSPQo4MT0"
// #define MQTT_PASSWORD "p5DLueSx4/FtzeL6MYlD9X/o"

#define HTTP_TIMEUP 30000
#define HTTP_API_KEY "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJleHAiOjE2NDAxNTM4ODAsImlhdCI6MTY0MDE0NjY4MCwicm9sZSI6InVzZXIiLCJ1c3IiOiJUb2RkIn0.XZdAz-SARo9qHik2DL07zPzgG6Cp4muqmwo0mfyVSCQ"
#define HTTP_URL_POST "https://backend.thinger.io/v3/users/Todd/devices/ESP32/callback/data"

#endif