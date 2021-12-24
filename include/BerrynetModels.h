#ifndef BERRYNETMODELS_H
#define BERRYNETMODELS_H

struct ModelSensors {
    struct Environment {
        float humidity = 0;
        float temperature = 0;
    } environment;

    struct Soil {
        uint16_t moisture = 0;
        float temperature = 0;  
    } soil;

    uint16_t light = 0;
};

#endif