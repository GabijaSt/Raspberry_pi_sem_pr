#include "LightSensorLDR.h"   
#include <cmath>
#include <algorithm>                                                    // Class declaration

LightSensorLDR::LightSensorLDR()
    : ADC(27) {}

float LightSensorLDR::readLux() {
    int raw = readAvg(5);
    if (raw <= 0) raw = 1;  
    const float Rfixed = 10000.0f;  // Fixed resistor in voltage divider (Ω)
    float rLDR = Rfixed * (static_cast<float>(raw) / (4095.0f - raw));

    const float A = 2.0e6f;
    const float gamma = 1.4f;

    float lux = powf((A / rLDR), 1.0f / gamma);
    return lux;
}