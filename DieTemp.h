#ifndef DIETEMP_H
#define DIETEMP_H

#include "ADC.h"                                                           // Base ADC class

class DieTemp : public ADC {
public:
    DieTemp();                                                             // Constructor (uses ADC channel 4)
    float readCelsius();                                                   // Returns temperature in °C
    float readFahrenheit();                                                // Returns temperature in °F
};

#endif // DIETEMP_H