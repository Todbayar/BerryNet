#pragma once

struct ModelSensors {
  struct Environment {
      float humidity;
      float temperature;
  };
  struct Soil {
      uint16_t moisture;
      float temperature;  
  };
};