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
  //lv_label_set_text_fmt(ui_Set_Temperature, "%d", setTemperature);
}

void setTemp(int8_t temp)
{
  deviceConfig.setTemp = temp;
  if (deviceConfig.setTemp < 0)
  {
    deviceConfig.setTemp = 0;
  }

  if (deviceConfig.setTemp > 100)
  {
    deviceConfig.setTemp = 100;
  }
  //lv_label_set_text_fmt(ui_Set_Temperature, "%d", setTemperature);
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

#ifdef DHT_ENABLED
  if (sensor_loop() == READ_UPDATE) {
    // Update temperature display
    // lv_label_set_text_fmt(temperature, "%.1f", temperature);
    // lv_label_set_text_fmt(humidity, "%.1f%%", humidity);
  }
#endif

#ifdef LCD_ENABLED
  lcd_loop();
#endif

  // if (millis() % 1000 < 10)
  // {
  //   // Update temperature display every second
  //   lv_label_set_text_fmt(ui_Temperature, "%d", temp++);
  // }
  // delay(5); // Add a small delay to allow for smoother updates
}
