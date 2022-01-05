#include <Arduino.h>
#include <U8g2lib.h>
#include <BH1750.h>
#include <DS18B20.h>
#include <OneWire.h>
#include "DHTesp.h"
#include "gravity_soil_moisture_sensor.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiUdp.h>
#include <ArduinoJson.h>
#include <NTPClient.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#define BUILTIN_LED 2
#define pinSensorEnv 23
#define pinSensorSoilMoist 35 //Must be Analog pin
#define pinSensorSoilTemp 32
#define pinRelayLight 26
#define pinRelayHeater 15
#define pinRelayWaterMotor 27
#define pinWaterMeter 14

#define addressBH1750 0x23
#define wifiSSID "AndroidAPBADD"
#define wifiPASS "ahnd4523"
#define HTTP_API_KEY "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJqdGkiOiJUaGluZ2VyRGV2aWNlQXV0aCIsInN2ciI6ImFwLXNvdXRoZWFzdC5hd3MudGhpbmdlci5pbyIsInVzciI6IlRvZGQifQ.UzPa-R28Ye-_fW7U1Q43ryHEw-NWP09WoK-qg9BaiC8"
#define HTTP_URL_POST "http://backend.thinger.io/v3/users/Todd/devices/ESP32/callback/data"

U8G2_SSD1306_128X64_NONAME_F_HW_I2C oled(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);   //full buffer
//U8G2_SSD1306_128X64_NONAME_1_HW_I2C oled(U8G2_R0, /* reset=*/ U8X8_PIN_NONE); //page buffer
BH1750 meterLight(addressBH1750);
OneWire oneWire(pinSensorSoilTemp);
DS18B20 meterSoilTemp(&oneWire);
DHTesp meterEnv;
GravitySoilMoistureSensor meterSoilMoist;

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "mn.pool.ntp.org", 3600, 60000);

TaskHandle_t task1;

uint16_t vLight, vEnvHumi, vEnvTemp, vSoilMoist;
double vSoilTemp;  //need more precision for determining inclining or declining
String vStat;
volatile double vWaterMeasure = 0.0;
IPAddress vWiFiLocalIP;
unsigned long vTimeHttpPost;
unsigned long vTimeHttpCountDown;
#define vTimeHttpWait 3600000L //1hour

void postHTTP(){
  StaticJsonDocument<96> jsonData;    //Calculated by "ArduinoJSON" (web site)
  jsonData["light"] = vLight;
  jsonData["env_humi"] = vEnvHumi;
  jsonData["env_temp"] = vEnvTemp;
  jsonData["soil_moist"] = vSoilMoist;
  jsonData["soil_temp"] = vSoilTemp;
  String jsonStr;
  serializeJson(jsonData, jsonStr);
  //Serial.println(jsonStr);
  WiFiClient wifiClient;
  HTTPClient httpClient;
  httpClient.begin(wifiClient, HTTP_URL_POST);
  httpClient.addHeader("Content-Type", "application/json");
  httpClient.addHeader("Accept", "text/plain");
  httpClient.addHeader("Authorization", "Bearer " + String(HTTP_API_KEY));
  uint16_t httpResponseCode = httpClient.POST(jsonStr);
  if(httpResponseCode == HTTP_CODE_ACCEPTED || httpResponseCode == HTTP_CODE_OK){
      vStat = "POST OK";
  }
  else if(httpResponseCode == HTTP_CODE_UNAUTHORIZED){
      vStat = "POST AUTH!";
  }
  else {
      vStat = httpResponseCode;
  }
  httpClient.end();
}

void WiFiEventSuccess(WiFiEvent_t event, WiFiEventInfo_t info){
  if(WiFi.status() == WL_CONNECTED) {
    vWiFiLocalIP = WiFi.localIP();
    vStat = vWiFiLocalIP.toString();
    delay(1000);
    timeClient.begin();
    timeClient.setTimeOffset(28800);
    vStat = timeClient.getFormattedTime();
    delay(1000);
  }
}

void WiFiEventFail(WiFiEvent_t event, WiFiEventInfo_t info){
  vStat = "WiFi?";
  WiFi.disconnect();
  WiFi.reconnect();
}

void IRAM_ATTR waterCounter(){
  vWaterMeasure += 1000.0 / 5880.0; //mL
  vStat = String(vWaterMeasure) + "mL";
  if(vWaterMeasure >= 23.0){
    vWaterMeasure = 0.0;
    digitalWrite(pinRelayWaterMotor, HIGH);
  }
}

void setup() {
  vTimeHttpPost = millis();
  vTimeHttpCountDown = vTimeHttpWait;

  oled.begin();
  oled.enableUTF8Print();
  
  meterLight.begin(BH1750::CONTINUOUS_HIGH_RES_MODE);
  meterEnv.setup(pinSensorEnv, DHTesp::DHT11);
  meterSoilMoist.Setup(pinSensorSoilMoist);
  meterSoilTemp.begin();

  pinMode(BUILTIN_LED, OUTPUT);
  pinMode(pinRelayLight, OUTPUT);
  pinMode(pinRelayHeater, OUTPUT);
  pinMode(pinRelayWaterMotor, OUTPUT);
  pinMode(pinWaterMeter, INPUT_PULLUP);

  digitalWrite(pinRelayLight, HIGH);  //off
  digitalWrite(pinRelayHeater, HIGH);
  digitalWrite(pinRelayWaterMotor, HIGH);

  vWaterMeasure = 0.0;
  attachInterrupt(digitalPinToInterrupt(pinWaterMeter), waterCounter, RISING);

  btStop();
  WiFi.onEvent(WiFiEventSuccess, SYSTEM_EVENT_STA_GOT_IP);
  WiFi.onEvent(WiFiEventFail, SYSTEM_EVENT_STA_DISCONNECTED);
  WiFi.mode(WIFI_STA);
  WiFi.begin(wifiSSID, wifiPASS);
}

void loop() {
  meterSoilTemp.requestTemperatures();
  vLight = meterLight.readLightLevel();
  vEnvHumi = meterEnv.getHumidity();
  vEnvTemp = meterEnv.getTemperature();
  vSoilMoist = meterSoilMoist.Read()*100/3500;
  vSoilTemp = meterSoilTemp.getTempC();

  //LIGHT - Turn on to morning 9 from night 17
  if(timeClient.getHours() <= 9 || timeClient.getHours() >= 17){
    digitalWrite(pinRelayLight, LOW); //on
  }
  else {
    //day time turn light off
    digitalWrite(pinRelayLight, HIGH); //off
  }

  //SOIL TEMP
  if(vSoilTemp < 22){
    digitalWrite(pinRelayHeater, LOW);
  }
  else if(vSoilTemp > 22 || vSoilTemp <= 23) {
    digitalWrite(pinRelayHeater, HIGH);
  }

  if(millis() >= vTimeHttpPost + vTimeHttpWait){
    if(WiFi.status() == WL_CONNECTED) {
      if(!timeClient.update()) timeClient.forceUpdate();
      postHTTP();
      delay(1000);
    }
    vTimeHttpPost = millis();
    vTimeHttpCountDown = vTimeHttpWait;

    //SOIL MOISTURE
    if(vSoilMoist < 40){
      digitalWrite(pinRelayWaterMotor, LOW);
    }
    else if(vSoilMoist >= 41 || vSoilMoist < 50){
      digitalWrite(pinRelayWaterMotor, HIGH);
    }
  }
  else {
    unsigned long vTimerCount = vTimeHttpCountDown / 1000;
    uint16_t vTimerHour = vTimerCount / 3600;                                     //hours
    uint16_t vTimerMinute = (vTimerCount - vTimerHour * 3600) / 60;               //minutes
    uint16_t vTimerSecond = vTimerCount - vTimerHour * 3600 - vTimerMinute * 60;  //seconds           
    vStat = String(vTimerHour) + ":" + String(vTimerMinute) + ":" + String(vTimerSecond);
    vTimeHttpCountDown -= 1000;
  }

  digitalWrite(BUILTIN_LED, HIGH);
  vTaskDelay(500);
  digitalWrite(BUILTIN_LED, LOW);
  vTaskDelay(500);

  oled.clearBuffer();
  oled.setFont(u8g2_font_unifont_t_cyrillic);
  oled.setFontDirection(0);

  oled.setCursor(0, 15);
  oled.print(F("Төлөв:"));
  oled.print(vStat);

  oled.setCursor(0, 30);
  oled.print(F("Гэрэл:"));
  oled.print(vLight);
  oled.print(F("LX"));

  oled.setCursor(0, 45);
  oled.print(F("Орчин:"));
  oled.print(vEnvHumi);
  oled.print(F("% "));
  oled.print(vEnvTemp);
  oled.print(F("C"));

  oled.setCursor(0, 60);
  oled.print(F("Хөрс:"));
  oled.print(vSoilMoist);
  oled.print(F("% "));
  oled.print(vSoilTemp);
  oled.print(F("C"));

  oled.sendBuffer();
}