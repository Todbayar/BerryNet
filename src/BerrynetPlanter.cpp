#include "Arduino.h"
#include "analogWrite.h"
#include "BerrynetPlanter.h"

BerrynetPlanter::BerrynetPlanter(uint16_t _pinLight, uint16_t _pinHeater, /* PWM */ uint16_t _pinWaterPump, uint16_t _pinWaterMeter){
    this->pinLight = _pinLight;
    this->pinHeater = _pinHeater;
    this->pinWaterPump = _pinWaterPump;
    this->pinWaterMeter = _pinWaterMeter;

    //Relays've been used as controlling AC devices
    pinMode(this->pinLight, OUTPUT);
    pinMode(this->pinHeater, OUTPUT);
    digitalWrite(this->pinLight, HIGH);
    digitalWrite(this->pinHeater, HIGH);

    pinMode(this->pinWaterPump, OUTPUT);    //L296 Motor driver for pump water
    pinMode(this->pinWaterMeter, INPUT);    //YF-S401 Water flow sensor
    digitalWrite(this->pinWaterPump, LOW);
    digitalWrite(this->pinWaterMeter, LOW);
};

bool BerrynetPlanter::Heater(BerrynetPlanter::turn_t _turn){
    switch(_turn){
        case ON:
            digitalWrite(this->pinHeater, LOW);
            return true;    //Need to determine exactly using read pin status
            break;
        case OFF:
            digitalWrite(this->pinHeater, HIGH);
            return false;
            break;
        default:
            digitalWrite(this->pinHeater, HIGH);
            return false;
            break;
    };
};

bool BerrynetPlanter::Light(BerrynetPlanter::turn_t _turn){
    switch(_turn){
        case ON:
            digitalWrite(this->pinLight, LOW);
            return true;
            break;
        case OFF:
            digitalWrite(this->pinLight, HIGH);
            return false;
            break;
        default:
            digitalWrite(this->pinLight, HIGH);
            return false;
            break;
    };
};

//Need to use relay for water pump and measure water flow meter YF-S401
//This function must be in xTaskCreate (FreeRTOS) for loop without using while, for  
ulong BerrynetPlanter::Pump(/*in millis*/ ulong _waitMinute, /*in millis*/ ulong _runSecond){
    digitalWrite(pinWaterPump, LOW);
    // if(this->pumpWaitMinute == 0){
    //     this->pumpWaitMinute = _waitMinute;
    //     this->pumpLastTime = millis();
    //     long vRunSecond = millis();
    //     uint16_t vRunSpeed = 0;
    //     do {
    //         analogWrite(this->pinWaterPump, vRunSpeed < 127 ? vRunSpeed++ : 127);   //LM298N VCC-12v to 6v 50%
    //         uint16_t vWaterMeterValue = digitalRead(this->pinWaterMeter);
    //         //Serial.printf("Water flow read:%u\r\n", vWaterMeterValue);
    //     }
    //     while(millis() - vRunSecond <= _runSecond);
    //     analogWrite(this->pinWaterPump, 0);
    // }
    // else {
    //     if(millis() - pumpLastTime > _waitMinute){
    //         this->pumpWaitMinute = 0;
    //     }
    // }
    // //Serial.printf("Time left to pump:%u\r\n", (pumpWaitMinute / 1000));
    // this->pumpWaitMinute -= 1;
    // return this->pumpWaitMinute;
    return 0;
}