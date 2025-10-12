//
//    FILE: DHT20.ino
//  AUTHOR: Rob Tillaart
// PURPOSE: Demo for DHT20 I2C humidity & temperature sensor
//     URL: https://github.com/RobTillaart/DHT20
//
//  Always check datasheet - front view
//
//          +--------------+
//  VDD ----| 1            |
//  SDA ----| 2    DHT20   |
//  GND ----| 3            |
//  SCL ----| 4            |
//          +--------------+

#include "DHT20.h"
#include <lvgl.h>
#include <TFT_eSPI.h> // Hardware-specific library
#include "ui/ui.h"

/*Set to your screen resolution and rotation*/
#define TFT_HOR_RES 320
#define TFT_VER_RES 480
#define TFT_ROTATION LV_DISPLAY_ROTATION_0

#define DRAW_BUF_SIZE (TFT_HOR_RES * TFT_VER_RES / 10 * (LV_COLOR_DEPTH / 8))

// #define DHT_ENABLED

uint32_t lcd_drawBufffer[DRAW_BUF_SIZE / 4];

DHT20 DHT;

uint8_t count = 0;

uint32_t lcd_tickProvider()
{
  return millis();
}

void setup()
{
  Serial.begin(115200);
  Serial.println(__FILE__);
  Serial.print("DHT20 LIBRARY VERSION: ");
  Serial.println(DHT20_LIB_VERSION);
  Serial.println();

#ifdef DHT_ENABLED
  Wire.begin();
  DHT.begin(); //  ESP32 default pins 21 22
#endif

  // delay(1000);

  lv_init();

  /*Set a tick source so that LVGL will know how much time elapsed. */
  lv_tick_set_cb(lcd_tickProvider);

  /*TFT_eSPI can be enabled lv_conf.h to initialize the display in a simple way*/
  lv_display_t *disp = lv_tft_espi_create(TFT_HOR_RES, TFT_VER_RES, lcd_drawBufffer, sizeof(lcd_drawBufffer));
  lv_display_set_rotation(disp, LV_DISPLAY_ROTATION_90);

  ui_init();
  Serial.println("Setup done");

  // lv_timer_create(lcd_updateCookTime, 1000, NULL); // Create a timer to update the temperature every second
}

void loop()
{

#ifdef DHT_ENABLED
  if (millis() - DHT.lastRead() >= 1000)
  {
    //  READ DATA
    uint32_t start = micros();
    int status = DHT.read();
    uint32_t stop = micros();

    if ((count % 10) == 0)
    {
      count = 0;
      Serial.println();
      Serial.println("Type\tHumidity (%)\tTemp (°C)\tTime (µs)\tStatus");
    }
    count++;

    Serial.print("DHT20 \t");
    //  DISPLAY DATA, sensor has only one decimal.
    Serial.print(DHT.getHumidity(), 1);
    Serial.print("\t\t");
    Serial.print(DHT.getTemperature(), 1);
    Serial.print("\t\t");
    Serial.print(stop - start);
    Serial.print("\t\t");
    switch (status)
    {
    case DHT20_OK:
      Serial.print("OK");
      break;
    case DHT20_ERROR_CHECKSUM:
      Serial.print("Checksum error");
      break;
    case DHT20_ERROR_CONNECT:
      Serial.print("Connect error");
      break;
    case DHT20_MISSING_BYTES:
      Serial.print("Missing bytes");
      break;
    case DHT20_ERROR_BYTES_ALL_ZERO:
      Serial.print("All bytes read zero");
      break;
    case DHT20_ERROR_READ_TIMEOUT:
      Serial.print("Read time out");
      break;
    case DHT20_ERROR_LASTREAD:
      Serial.print("Error read too fast");
      break;
    default:
      Serial.print("Unknown error");
      break;
    }
    Serial.print("\n");
  }

#endif

  lv_task_handler(); // Call the LVGL task handler to process events and draw
  delay(5);          // Add a small delay to allow for smoother updates
}

//  -- END OF FILE --