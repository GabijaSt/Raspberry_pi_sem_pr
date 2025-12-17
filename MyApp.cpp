// Must include lwIP C headers first (wrapped in extern "C")
extern "C" {
#include "lwip/ip_addr.h"
#include "lwip/dns.h"
#include "lwip/apps/mqtt.h"
#include "lwip/apps/mqtt_priv.h"
}
#include <string.h>
#include "MyApp.h"
#include <iostream>
#include <iomanip>
#include <cstring>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"

#define WIFI_SSID "Ola"
#define WIFI_PASS "s7urbnupsymc2"
#define MQTT_TOPIC "pico/sensors"

using namespace std;

// Forward declarations
static void dnsCallback(const char *name, const ip_addr_t *addr, void *arg);
static void mqtt_connection_cb(mqtt_client_t* client, void* arg, mqtt_connection_status_t status);

// -----------------------------------------------------------------------------
// Constructor
// -----------------------------------------------------------------------------
MyApp::MyApp()
    : shtc3(i2c0)
{
    printf("Initializing MyApp...\n");

    i2c_init(i2c0, 400000);
    gpio_set_function(4, GPIO_FUNC_I2C);
    gpio_set_function(5, GPIO_FUNC_I2C);
    gpio_pull_up(4);
    gpio_pull_up(5);

    if (!shtc3.init()) {
        printf("SHTC3 init failed\n");
    } else {
        printf("SHTC3 ready\n");
    }

    // Init WiFi hardware
    if (cyw43_arch_init()) {
        printf("WiFi init failed!\n");
        return;
    }
    cyw43_arch_enable_sta_mode();

    printf("Connecting WiFi...\n");

    if (cyw43_arch_wifi_connect_timeout_ms(
            WIFI_SSID, WIFI_PASS,
            CYW43_AUTH_WPA2_AES_PSK,
            30000))
    {
        printf("WiFi connect failed!\n");
    } 
    else {
        printf("WiFi connected.\n");
    }

    mqttInit();
}

// -----------------------------------------------------------------------------
// DNS + MQTT Initialization
// -----------------------------------------------------------------------------
void MyApp::mqttInit()
{
    printf("Resolving MQTT broker (broker.hivemq.com)...\n");
    err_t err = dns_gethostbyname("broker.hivemq.com", &brokerIp, dnsCallback, this);

    if (err == ERR_OK) {
        // Cached DNS result -> immediate connect
        connectToBroker(brokerIp);
    } else if (err == ERR_INPROGRESS) {
        printf("DNS query in progress...\n");
    } else {
        printf("DNS lookup error: %d\n", err);
    }
}

// -----------------------------------------------------------------------------
// Connect to broker (called from DNS callback or directly)
// -----------------------------------------------------------------------------
void MyApp::connectToBroker(const ip_addr_t &ip)
{
    printf("Connecting to MQTT broker at: %s\n", ipaddr_ntoa(&ip));

    // Free any existing client first
    if (mqttClient) {
        // safe disconnect if needed (optional)
        mqtt_disconnect(mqttClient);
        mqtt_client_free(mqttClient);
        mqttClient = nullptr;
        mqttConnected = false;
    }

    mqttClient = mqtt_client_new();
    if (!mqttClient) {
        printf("Failed to allocate MQTT client.\n");
        return;
    }

    static mqtt_connect_client_info_t ci;
    memset(&ci, 0, sizeof(ci));
    ci.client_id = "pico_oop_client";

    // Use the C-style connection function and callback
    mqtt_client_connect(
        mqttClient,
        &ip,
        1883,
        mqtt_connection_cb,
        this,
        &ci
    );
}

// -----------------------------------------------------------------------------
// Publish JSON message
// -----------------------------------------------------------------------------
void MyApp::mqttPublish(float tempC, float lightLux, float humidity)
{
    if (!mqttClient || !mqttConnected) return;

    char json[128];
    int n = snprintf(json, sizeof(json),
        "{\"temperature\": %.2f, \"light\": %.2f, \"humidity\": %.2f}", tempC, lightLux, humidity);
    if (n < 0) return;

    err_t err = mqtt_publish(
        mqttClient,
        MQTT_TOPIC,
        json, strlen(json),
        0, 
        0,
        nullptr, nullptr
    );

    if (err == ERR_OK) {
        printf("Published: %s (OK)\n", json);
    } else {
        printf("ERROR: MQTT Publish failed with code: %d\n", err); 
    }

    printf("Published: %s\n", json);
}

// -----------------------------------------------------------------------------
// DNS Callback (called by lwIP when DNS resolves)
// -----------------------------------------------------------------------------
static void dnsCallback(const char *name, const ip_addr_t *addr, void *arg)
{
    MyApp* app = static_cast<MyApp*>(arg);
    if (!app) return;

    if (addr) {
        printf("DNS resolved: %s -> %s\n", name, ipaddr_ntoa(addr));
        app->connectToBroker(*addr);
    } else {
        printf("DNS lookup failed for %s.\n", name);
    }
}

// -----------------------------------------------------------------------------
// MQTT connection callback — lwIP invokes this when connect result arrives
// -----------------------------------------------------------------------------
static void mqtt_connection_cb(mqtt_client_t* client, void* arg, mqtt_connection_status_t status)
{
    MyApp* app = static_cast<MyApp*>(arg);
    if (!app) return;

    if (status == MQTT_CONNECT_ACCEPTED) {
        printf("MQTT connected successfully.\n");
        app->setMqttConnected(true);
    } else {
        printf("MQTT connection failed: %d\n", status);
        app->setMqttConnected(false);
    }
}

// -----------------------------------------------------------------------------
// Simple setter/getter for mqttConnected (public API)
// -----------------------------------------------------------------------------
void MyApp::setMqttConnected(bool v)
{
    mqttConnected = v;
}

bool MyApp::isMqttConnected() const
{
    return mqttConnected;
}

// -----------------------------------------------------------------------------
// Main loop — read sensors and publish
// -----------------------------------------------------------------------------
void MyApp::run()
{
    while (true)
    {
        float tempC = dieTemp.readCelsius();
        float light = lightSensor.readLux();
        float humidity;

        if (shtc3.read(humidity)) {
            printf("RH: %.2f %%\n", humidity);
        } else {
            printf("SHTC3 read failed\n");
        }

        mqttPublish(tempC, light, humidity);

        printf("Temp: %.1f °C | Light: %.1f lux\n | RH: %.2f %%\n", tempC, light, humidity);

        sleep_ms(2000);  // publish once per second
    }
}
