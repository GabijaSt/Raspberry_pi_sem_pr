 #include <stdio.h>
 #include "pico/stdlib.h"
 #include "hardware/i2c.h"


//=========================================================================  
//  SHTC3.cpp
//  Direct I²C driver for SHTC3 (no Wire API)                               
//=========================================================================  

#include "SHTC3.h"
#include <cmath>

//-------------------------------------------------------------------------
// Constructor
//-------------------------------------------------------------------------
SHTC3::SHTC3(i2c_inst_t* i2c, uint8_t addr)
    : _i2c(i2c), _addr(addr)
{
}

//-------------------------------------------------------------------------
// Send 16-bit command (MSB first)
//-------------------------------------------------------------------------
bool SHTC3::sendCommand(uint16_t cmd)
{
    uint8_t buf[2] = {
        static_cast<uint8_t>(cmd >> 8),
        static_cast<uint8_t>(cmd & 0xFF)
    };

    int written = i2c_write_blocking(_i2c, _addr, buf, 2, false);
    return (written == 2);
}

//-------------------------------------------------------------------------
// Read raw bytes
//-------------------------------------------------------------------------
bool SHTC3::readBytes(uint8_t* buf, size_t len)
{
    int read = i2c_read_blocking(_i2c, _addr, buf, len, false);
    return (read == static_cast<int>(len));
}

//-------------------------------------------------------------------------
// Wake device
//-------------------------------------------------------------------------
bool SHTC3::wake()
{
    return sendCommand(SHTC3_CMD_WAKEUP);
}

//-------------------------------------------------------------------------
// Sleep device
//-------------------------------------------------------------------------
bool SHTC3::sleep()
{
    return sendCommand(SHTC3_CMD_SLEEP);
}

//-------------------------------------------------------------------------
// Initialization
//-------------------------------------------------------------------------
bool SHTC3::init()
{
    sleep_ms(1);
    return wake();
}

//-------------------------------------------------------------------------
// Read temperature and humidity
//-------------------------------------------------------------------------
bool SHTC3::read(float& humidity_rh)
{
    if (!wake()) return false;

    if (!sendCommand(SHTC3_CMD_MEAS_T_RH))
        return false;

    sleep_ms(15);     // Measurement time (datasheet: 12.1 ms typical)

    uint8_t buf[6];
    if (!readBytes(buf, 6))
        return false;
        
    uint16_t rawH = (buf[3] << 8) | buf[4];

    humidity_rh   = 100.0f * ((float)rawH / 65535.0f);

    sleep();
    return true;
}



// int main()
// {
//     stdio_init_all();

//     i2c_init(i2c0, 400000);
//     gpio_set_function(4, GPIO_FUNC_I2C);
//     gpio_set_function(5, GPIO_FUNC_I2C);
//     gpio_pull_up(4);
//     gpio_pull_up(5);

//     SHTC3 sensor(i2c0);

//     if (!sensor.init()) {
//         printf("SHTC3 init failed\n");
//         return 0;
//     }

//     while (1) {
//         float t, h;
//         if (sensor.read(t, h)) {
//             printf("T: %.2f C,  RH: %.2f %%\n", t, h);
//         }
//         sleep_ms(1000);
//     }
// }
