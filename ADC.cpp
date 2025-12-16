#include <cstdint>                                                          // Fixed-width integer types (e.g., uint16_t)
#include "ADC.h"                                                            // Class declaration
#include "hardware/adc.h"                                                   // Pico SDK ADC hardware functions
#include "dbop.h"                                                           // Debug macros (C_ADC)

ADC::ADC(uint gpioPin) : _pin(gpioPin) {                                    C_ADC("ADC constructor called");
    init();                                                                 // Initialize ADC hardware and configure input pin
}

void ADC::init() {
    adc_init();                                                             C_ADC("Initialize ADC hardware");
    adc_gpio_init(_pin);                                                    C_ADC("Connect GPIO pin to ADC input");

    // Map GPIO pin to ADC channel
    if      (_pin == 30) _channel = 4;                                      // Internal temperature sensor
    else if (_pin >= 26 && _pin <= 28) _channel = _pin - 26;                // GPIO26–28 → ADC0–2
    else _channel = 0;                                                      // Default fallback if invalid pin is used

    adc_select_input(_channel);                                             C_ADC("Select ADC input channel");
}

int ADC::read() {
    adc_select_input(_channel);                                             C_ADC("Select ADC channel before reading");
    uint16_t raw = adc_read();                                              C_ADC("Read raw 12-bit ADC value");
    return static_cast<int>(raw);                                           // Return ADC result as integer
}

int ADC::readAvg(int avgVal) {                                              C_ADC("Read averaged ADC value");
    if (avgVal <= 0) avgVal = 1;                                            // Ensure at least one sample is read

    uint32_t sum = 0;                                                       // Accumulator for summed ADC readings
    for (int i = 0; i < avgVal; i++) {                                      // Repeat for the number of samples
        sum += read();                                                      // Read one ADC value and add to sum
        sleep_ms(2);                                                        // Small delay for stable sampling
    }

    int avg = static_cast<int>(sum / avgVal);                               C_ADC("Return averaged ADC value");
    return avg;
}
