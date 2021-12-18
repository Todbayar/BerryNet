#include <Arduino.h>
#include <U8g2lib.h>    //oled 0.96" 128x64 noname i2c
// #include <BerrynetSensors.h>

U8G2_SSD1306_128X64_NONAME_F_HW_I2C oled(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);
//Sensors sensors;

void setup() {
    Serial.begin(115200);
    while(!Serial);

    oled.begin();
    oled.enableUTF8Print();
    //oled.setFont(u8g2_font_4x6_t_cyrillic);
    oled.setFont(u8g2_font_unifont_t_cyrillic);
    oled.setFontDirection(0);
}

void loop() {
    // Model_Sensors varSensors = sensors.Read();
    // oled.clearBuffer();
    // oled.setCursor(0, 15);
    // oled.printf("Light:");
    // oled.setCursor(0, 30);
    // oled.print("Сайн байна уу?");
    // oled.setCursor(0, 45);
    // oled.print("Дэлхий минь!");
    // oled.setCursor(0, 60);
    // oled.print("Ө, Ү");
    // oled.sendBuffer();
}