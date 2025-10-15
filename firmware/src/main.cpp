#include <Arduino.h>
#include "configuration.h"
#include "DHT20.h"
#include "sensor.h"
#include "lcd.h"

DeviceConfig deviceConfig;

float temperature = 0;
float humidity = 0;

void incrementTemp(int8_t delta)
{
  deviceConfig.setTemp += delta;
  if (deviceConfig.setTemp < 0)
  {
    deviceConfig.setTemp = 0;
  }

  if (deviceConfig.setTemp > 100)
  {
    deviceConfig.setTemp = 100;
  }

  deviceConfig.dirty = true;
}

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

#ifdef LCD_ENABLED
  lcd_setup();
#endif

#ifdef DHT_ENABLED
  sensor_setup(&humidity, &temperature);
#endif
}

void loop()
{

// Called before saving configure to update UI if deviceConfig changed
#ifdef LCD_ENABLED
  lcd_loop();
#endif

  if (deviceConfig.dirty)
  {
    deviceConfig.dirty = false;
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
  }

#ifdef DHT_ENABLED
  sensor_loop();
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
