#ifndef LIGHTSENSORLDR_H
#define LIGHTSENSORLDR_H

#include "ADC.h"                                                           // Base ADC class

class LightSensorLDR : public ADC {
public:
    LightSensorLDR();                                                      // Constructor (fixed to GPIO28)
    float readLux();                                                       // Returns approximate light intensity in lux

private:
    const float Rfixed = 10000.0f;                                         // Fixed resistor in voltage divider (Ω)
};

#endif // LIGHTSENSORLDR_H

