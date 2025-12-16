#ifndef ADC_H
#define ADC_H

#include "pico/stdlib.h"                                                    // Pico SDK base definitions

class ADC {
public:
    ADC(uint gpioPin);                                                      // Constructor (defines ADC input pin)
    int  read();                                                            // Reads one raw 12-bit ADC value
    int  readAvg(int avgVal);                                               // Returns averaged ADC reading

protected:
    uint _pin;                                                              // GPIO pin number for ADC input
    uint _channel;                                                          // ADC channel number
    void init();                                                            // Initializes ADC hardware and channel
};

#endif // ADC_H
