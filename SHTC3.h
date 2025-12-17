//=========================================================================  
//  SHTC3.h
//  Driver for Sensirion SHTC3 Temperature & Humidity Sensor (I²C)         
//=========================================================================  

#ifndef SHTC3_H
#define SHTC3_H

#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include <cstdint>

//-------------------------------------------------------------------------
//  SHTC3 Constants (from datasheet)
//-------------------------------------------------------------------------
constexpr uint8_t  SHTC3_I2C_ADDR        = 0x70;     // 7-bit address

// Wake / Sleep commands
constexpr uint16_t SHTC3_CMD_WAKEUP      = 0x3517;
constexpr uint16_t SHTC3_CMD_SLEEP       = 0xB098;

// Measurement commands (normal mode)
constexpr uint16_t SHTC3_CMD_MEAS_T_RH   = 0x7866;   // Temp first, then RH

class SHTC3 {
public:
    SHTC3(i2c_inst_t* i2c, uint8_t addr = SHTC3_I2C_ADDR);

    bool init();
    bool read(float& humidity_rh);

private:
    bool wake();
    bool sleep();
    bool sendCommand(uint16_t cmd);
    bool readBytes(uint8_t* buf, size_t len);

    i2c_inst_t* _i2c;
    uint8_t _addr;
};

#endif
