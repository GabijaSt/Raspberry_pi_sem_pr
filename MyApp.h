#ifndef MYAPP_H
#define MYAPP_H

#include "KeyPress.h"
#include "LightSensorLDR.h"
#include "DieTemp.h"

#include "hardware/i2c.h"
#include "SHTC3.h"

extern "C" {
#include "lwip/apps/mqtt.h"
#include "lwip/ip_addr.h"
}

#include "pico/cyw43_arch.h"

class MyApp {
public:
    MyApp();                                // Constructor
    void run();                             // Main loop (publishes periodically)

    // Public API for connection helper and status access
    void connectToBroker(const ip_addr_t &ip);   // Connect to broker (called from DNS callback)
    void setMqttConnected(bool v);               // Setter used by callbacks
    bool isMqttConnected() const;                // Getter

private:
    // PicoBricks hardware / sensors
    KeyPress        keyPress;
    LightSensorLDR  lightSensor;
    DieTemp         dieTemp;

    // MQTT / network state
    mqtt_client_t*  mqttClient = nullptr;
    ip_addr_t       brokerIp{};
    bool            mqttConnected = false;

    // Internal helpers
    void mqttInit();
    void mqttPublish(float tempC, float lightLux, float humidity);

    SHTC3 shtc3; 
};

#endif // MYAPP_H
