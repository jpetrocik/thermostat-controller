#include <Arduino.h>
#include "configuration.h"
#include "DHT20.h"
#include "sensor.h"
#include "lcd.h"
#include "storage.h"

DeviceConfig deviceConfig;

float temperature = 0;
float humidity = 0;

bool uiStale = true;

DeviceConfig *currentDeviceConfig()
{
  return &deviceConfig;
}

void heatEnabled(bool on)
{
  deviceConfig.heatEnabled = on;
  Serial.println(on ? "Heating On" : "Heating Off");
}

void setup()
{
  Serial.begin(115200);
  Serial.println("Thermostat Controller");

  Wire.begin();

#ifdef LITTLEFS_ENABLED
  if (loadDeviceConfig(&deviceConfig))
  {
    Serial.println("Saved Configuration:");
    Serial.printf(" Device: %s\n", deviceConfig.deviceName);
    Serial.printf(" Room: %s\n", deviceConfig.roomName);
    Serial.printf(" Location: %s\n", deviceConfig.locationName);
    Serial.printf(" Hostname: %s\n", deviceConfig.hostname);
    Serial.printf(" MQTT Host: %s\n", deviceConfig.mqttHost);
    Serial.printf(" WiFi SSID: %s\n", deviceConfig.wifiSsid);
    Serial.printf(" Set Temp: %d\n", deviceConfig.setTemp);
    Serial.printf(" Heat Enabled: %s\n", deviceConfig.heatEnabled ? "true" : "false");
    Serial.printf(" Hysteresis: %d\n", deviceConfig.hysteresis);
  }
  else
  {
    Serial.println("Using default device configuration");
  }
#endif

#ifdef LCD_ENABLED
  lcd_setup();
#endif

#ifdef DHT_ENABLED
  sensor_setup(&humidity, &temperature);
#endif
}

void loop()
{

#ifdef LCD_ENABLED
  lcd_loop(uiStale);
  uiStale = false;
#endif

  if (deviceConfig.dirty)
  {
    // Clamp setTemp to valid range
    deviceConfig.setTemp = constrain(deviceConfig.setTemp, 0, 99);

    // Persist configuration
#ifdef LITTLEFS_ENABLED
    if (saveDeviceConfig(deviceConfig))
    {
      Serial.println("Configuration saved to LittleFS");
    }
    else
    {
      Serial.println("Failed to save configuration");
    }
#endif
    deviceConfig.dirty = false;

    // TODO Save configuration to persistent storage
    Serial.println("Configuration changed:");
    Serial.printf(" Device: %s\n", deviceConfig.deviceName);
    Serial.printf(" Room: %s\n", deviceConfig.roomName);
    Serial.printf(" Location: %s\n", deviceConfig.locationName);
    Serial.printf(" Hostname: %s\n", deviceConfig.hostname);
    Serial.printf(" MQTT Host: %s\n", deviceConfig.mqttHost);
    Serial.printf(" WiFi SSID: %s\n", deviceConfig.wifiSsid);
    Serial.printf(" Set Temp: %d\n", deviceConfig.setTemp);
    Serial.printf(" Heat Enabled: %s\n", deviceConfig.heatEnabled ? "true" : "false");
    Serial.printf(" Hysteresis: %d\n", deviceConfig.hysteresis);

    uiStale = true;
  }

#ifdef DHT_ENABLED
  if (sensor_loop()){
    uiStale = true;
  }
#endif

  if (!deviceConfig.heatEnabled)
  {
    // TODO Turn off heat relay
  }
  else
  {
    // TODO Control heat relay based on temperature and setTemp with hysteresis
  }
}
