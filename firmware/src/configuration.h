#ifndef Configuration_h
#define Configuration_h

#include <Arduino.h>
#include "relay.h"

#define CLIENT_ID "thermostat-%i"

#define LCD_ENABLED
#define DHT_ENABLED
// #define MQTT_ENABLED
// #define WEBSERVER_ENABLED
// #define OTA_ENABLED
// #define MDNS_ENABLED

#define RELAY_PIN 14
#define LED_PIN 2
#define BUTTON_PIN 0

#define LED_ON LOW
#define LED_OFF HIGH

// Determines how often the UI is updated
#define UI_UPDATE_INTERVAL 50

// Screen resolution and rotation
#define TFT_HOR_RES 320
#define TFT_VER_RES 480
#define TFT_ROTATION LV_DISPLAY_ROTATION_0

// Touchscreen pins
#define TOUCH_IRQ 22  // T_IRQ
#define TOUCH_MOSI 23 // T_DIN
#define TOUCH_MISO 19 // T_OUT
#define TOUCH_CLK 18  // T_CLK
#define TOUCH_CS 16   // T_CS

#define DRAW_BUF_SIZE (TFT_HOR_RES * TFT_VER_RES / 10 * (LV_COLOR_DEPTH / 8))

struct DeviceConfig
{
  char deviceName[20] = "thermostat";
  char roomName[20] = "livingroom";
  char locationName[20] = "house";
  char hostname[41] = "livingroom-thermostat";
  char mqttHost[50];
  char wifiSsid[25];
  char wifiPassword[50];
  int8_t setTemp = 45;
  bool dirty = false;
};

void incrementTemp(int8_t delta);
void setTemp(int8_t temp);

#endif
