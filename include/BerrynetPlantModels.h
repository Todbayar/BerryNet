#ifndef BERRYNETEXTERNALMODELS_H
#define BERRYNETEXTERNALMODELS_H

#include "Arduino.h"

struct ModelBerry {
    String name = "Strawberry";
    
    //Characteristics
    struct Days {
        const uint16_t germinate = 30;
    };

    //Condition for growing
    struct Light {
        const uint16_t min = 1400;
        const uint16_t max = 10000;
        const uint16_t opt = 3200;
    } light;

    struct Soil {
        struct Moisture {
            //percent, 730/3500*100 (20%) is dry 
            const uint16_t min = 40;
            const uint16_t max = 70;
            const uint16_t opt = 44;
        } moist;

        struct Temperature {
            const uint16_t min = 19;
            const uint16_t max = 30;
            const uint16_t opt = 21;
        } temp;

    } soil;

    struct Environment {
        //dust meter for avoiding mites
        struct Moisture {
            const uint16_t min = 40;
            const uint16_t max = 70;
            const uint16_t opt = 44;
        } moist;

        struct Temperature {
            const uint16_t min = 19;
            const uint16_t max = 30;
            const uint16_t opt = 24;
        } temp;

    } env;
};

#endif