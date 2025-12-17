#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "MyApp.h"       // Application class
#include "dbop.h"        // Debug macros (optional)
#include "hardware/i2c.h"
#include "SHTC3.h"
#include <cmath>

int main()
{
    stdio_init_all();

    MyApp app; // Create application object
    app.run(); // Start main application loop


   return 0;
}