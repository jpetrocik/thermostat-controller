#ifndef Configuration_h
#define Configuration_h

#include <Arduino.h>
#include "relay.h"

#define CLIENT_ID "thermostat-%i"

#define RELAY_PIN 14
#define LED_PIN 2
#define BUTTON_PIN 0

#define LED_ON LOW
#define LED_OFF HIGH

// #define DHT_ENABLED
// #define MQTT_ENABLED
// #define WEBSERVER_ENABLED
// #define OTA_ENABLED
// #define MDNS_ENABLED

struct DeviceConfig
{
  char deviceName[20] = "thermostat";
  char roomName[20] = "livingroom";
  char locationName[20] = "house";
  char hostname[41] = "livingroom-thermostat";
  char mqttHost[50];
  char wifiSsid[25];
  char wifiPassword[50];
  bool dirty = false;
};

#endif
