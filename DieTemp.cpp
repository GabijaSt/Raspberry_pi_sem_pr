#include "DieTemp.h"                                                       // Class declaration
#include "hardware/adc.h"                                                  // RP2040 ADC hardware functions

DieTemp::DieTemp()
    : ADC(30) {                                                            // GPIO29 → ADC4 (internal temperature)
    adc_set_temp_sensor_enabled(true);                                     // Enable internal temperature sensor
}

float DieTemp::readCelsius() {
    adc_set_temp_sensor_enabled(true);                                     // Enable internal temperature sensor
    uint16_t raw = readAvg(20);                                            // Average 20 samples for stability

    const float Vconv = 3.3f / 4095.0f;                                    // Conversion factor (voltage per step)
    float voltage = raw * Vconv;                                           // Convert ADC reading to voltage
    float tempC = 27.0f - (voltage - 0.706f) / 0.001721f;                  // Convert voltage to temperature (°C)

    return tempC;                                                          // Return temperature in °C
}

float DieTemp::readFahrenheit() {
    return readCelsius() * 9.0f / 5.0f + 32.0f;                            // °C → °F conversion
}