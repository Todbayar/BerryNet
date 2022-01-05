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

#define pinSensorEnv 23
#define pinSensorSoilMoist 35 //Must be Analog pin
#define pinSensorSoilTemp 32
#define pinRelayLight 26
#define pinRelayHeater 15
#define pinWaterMotor 27
#define pinWaterMeter 14

#define addressBH1750 0x23
#define wifiSSID "AndroidAPBADD"
#define wifiPASS "ahnd4523"
#define microsecToSec 1000000
#define timeToSleep 600 //10min
#define HTTP_TIMEUP 10000
#define HTTP_API_KEY "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpX*******"
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
RTC_DATA_ATTR int bootCount = 0;

uint16_t vLight, vEnvHumi, vEnvTemp, vSoilMoist, vSoilTemp;
String vStat;
volatile double vWaterMeasure = 0.0;
hw_timer_t * timerWater = NULL;
portMUX_TYPE timerWaterMux = portMUX_INITIALIZER_UNLOCKED;
IPAddress vWiFiLocalIP;
unsigned long vTimeHttpPost;
unsigned long vTimeHttpCountDown;
const unsigned long vTimeHttpWait = 3600000L; //1hour

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
  //Serial.println(httpResponseCode);
  httpClient.end();
}

void print_wakeup_reason(){
  esp_sleep_wakeup_cause_t wakeup_reason = esp_sleep_get_wakeup_cause();
  switch(wakeup_reason){
    case ESP_SLEEP_WAKEUP_EXT0: vStat = "Wakeup RTC_IO"; break;
    case ESP_SLEEP_WAKEUP_EXT1: vStat = "Wakeup RTC_CNTL"; break;
    case ESP_SLEEP_WAKEUP_TIMER: vStat = "Wakeup Timer"; break;
    case ESP_SLEEP_WAKEUP_ULP: vStat = "Wakeup ULP"; break;
    default: vStat = "Wakeup " + String(wakeup_reason); break;
  }
}

void WiFiEventSuccess(WiFiEvent_t event, WiFiEventInfo_t info){
  if(WiFi.status() == WL_CONNECTED) {
    vWiFiLocalIP = WiFi.localIP();
    vStat = vWiFiLocalIP.toString();
    timeClient.begin();
    timeClient.setTimeOffset(28800);
  }
}

void WiFiEventFail(WiFiEvent_t event, WiFiEventInfo_t info){
  vStat = "WiFi?";
  WiFi.disconnect();
  WiFi.reconnect();
}

void IRAM_ATTR waterCounter(){
  vWaterMeasure += 1000.0 / 5880.0; //mL
  vStat = vWaterMeasure;
  // if(vWaterMeasure >= 20){
  //   vWaterMeasure = 0;
  //   digitalWrite(pinWaterMotor, LOW);
  //   attachInterrupt(digitalPinToInterrupt(pinWaterMeter), waterCounter, FALLING);
  // }
}

void IRAM_ATTR waterOnTime(){
  portENTER_CRITICAL_ISR(&timerWaterMux);
  if(vSoilMoist < 40){
    digitalWrite(pinWaterMotor, LOW);  //pump
  }
  else {
    digitalWrite(pinWaterMotor, HIGH);  //off
  }
  portEXIT_CRITICAL_ISR(&timerWaterMux);
}

void setup() {
  vTimeHttpPost = millis();
  vTimeHttpCountDown = vTimeHttpWait;
  vStat = timeClient.getFormattedTime();
  delay(1000);
  print_wakeup_reason();
  esp_sleep_enable_timer_wakeup(timeToSleep * microsecToSec);

  oled.begin();
  oled.enableUTF8Print();
  
  meterLight.begin(BH1750::CONTINUOUS_HIGH_RES_MODE);
  meterEnv.setup(pinSensorEnv, DHTesp::DHT11);
  meterSoilMoist.Setup(pinSensorSoilMoist);
  meterSoilTemp.begin();

  pinMode(2, OUTPUT);
  pinMode(pinRelayLight, OUTPUT);
  pinMode(pinRelayHeater, OUTPUT);
  pinMode(pinWaterMotor, OUTPUT);
  pinMode(pinWaterMeter, INPUT_PULLUP);

  digitalWrite(pinRelayLight, HIGH);  //off
  digitalWrite(pinRelayHeater, HIGH);
  //digitalWrite(pinRelayWaterMotor, LOW);

  vWaterMeasure = 0;
  attachInterrupt(digitalPinToInterrupt(pinWaterMeter), waterCounter, FALLING);

  btStop();
  WiFi.onEvent(WiFiEventSuccess, SYSTEM_EVENT_STA_GOT_IP);
  WiFi.onEvent(WiFiEventFail, SYSTEM_EVENT_STA_DISCONNECTED);
  WiFi.mode(WIFI_STA);
  WiFi.begin(wifiSSID, wifiPASS);

  //digitalWrite(pinWaterMotor, HIGH);  //pump

  // Serial.begin(115200);
  // while(!Serial);
}

void loop() {
  if(WiFi.status() == WL_CONNECTED) {
    if(!timeClient.update()) timeClient.forceUpdate();
    
    if(millis() >= vTimeHttpPost + vTimeHttpWait){
      //Serial.println("HTTP SEND");
      delay(1000);
      postHTTP();
      vTimeHttpPost = millis();
      vTimeHttpCountDown = vTimeHttpWait;
    }
    else {
      unsigned long vTimerCount = vTimeHttpCountDown / 1000;
      uint16_t vTimerHour = vTimerCount / 3600;                                     //hours
      uint16_t vTimerMinute = (vTimerCount - vTimerHour * 3600) / 60;               //minutes
      uint16_t vTimerSecond = vTimerCount - vTimerHour * 3600 - vTimerMinute * 60;  //seconds           
      vStat = String(vTimerHour) + ":" + String(vTimerMinute) + ":" + String(vTimerSecond);
      vTimeHttpCountDown -= 1000;
    }
  }

  // Serial.print("Day:");
  // Serial.print(timeClient.getDay());
  // Serial.print(" House:");
  // Serial.print(timeClient.getHours());
  // Serial.print(" Time:");
  // Serial.println(timeClient.getFormattedTime());

  meterSoilTemp.requestTemperatures();
  
  vLight = meterLight.readLightLevel();
  vEnvHumi = meterEnv.getHumidity();
  vEnvTemp = meterEnv.getTemperature();
  vSoilMoist = meterSoilMoist.Read()*100/3500;
  vSoilTemp = meterSoilTemp.getTempC();

  //if(vLight < 1690 && (timeClient.getHours() >= 19 && timeClient.getHours() <= 7)){
  //else if(vLight > 1690 && vLight <= 6590) {
  //Turn light on to morning 9 from night 17
  if(timeClient.getHours() <= 9 || timeClient.getHours() >= 17){
    digitalWrite(pinRelayLight, LOW); //on
  }
  else {
    //day time turn light off
    digitalWrite(pinRelayLight, HIGH); //off
  }

  if(vSoilTemp < 22){
    digitalWrite(pinRelayHeater, LOW);
  }
  else if(vSoilTemp > 22 || vSoilTemp <= 23) {
    digitalWrite(pinRelayHeater, HIGH);
  }

  // timerWater = timerBegin(0, 80, true);
  // timerAttachInterrupt(timerWater, &waterOnTime, true);
  // timerAlarmWrite(timerWater, 30000000L, true);
  // timerAlarmEnable(timerWater);

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

  // Serial.print("Env humi:");
  // Serial.print(vEnvHumi);
  // Serial.print(", temp:");
  // Serial.print(vEnvTemp);
  // Serial.print(" Soil humi:");
  // Serial.print(vSoilMoist);
  // Serial.print(", temp:");
  // Serial.println(vSoilTemp);

  digitalWrite(2, HIGH);
  delay(500);
  digitalWrite(2, LOW);
  delay(500);

  // delay(1000);
  // Serial.flush();
  // vStat = "Sleep";
  // delay(4000);
  // esp_deep_sleep_start();
}
