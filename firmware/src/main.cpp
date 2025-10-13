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

#include <Arduino.h>
#include "DHT20.h"
#include <lvgl.h>
#include <TFT_eSPI.h> // Hardware-specific library
#include "ui/ui.h"
#include <XPT2046_Touchscreen.h>

/*Set to your screen resolution and rotation*/
#define TFT_HOR_RES 320
#define TFT_VER_RES 480
#define TFT_ROTATION LV_DISPLAY_ROTATION_0

#define TOUCH_CS 4
#define TOUCH_IRQ 22

// Touchscreen pins
#define XPT2046_IRQ 22  // T_IRQ
#define XPT2046_MOSI 23 // T_DIN
#define XPT2046_MISO 19 // T_OUT
#define XPT2046_CLK 18  // T_CLK
#define XPT2046_CS 16   // T_CS

#define DRAW_BUF_SIZE (TFT_HOR_RES * TFT_VER_RES / 10 * (LV_COLOR_DEPTH / 8))

// #define DHT_ENABLED

// SPIClass touchscreenSPI = SPIClass(VSPI);
XPT2046_Touchscreen touchscreen(XPT2046_CS, XPT2046_IRQ);

uint32_t lcd_drawBufffer[DRAW_BUF_SIZE / 4];

DHT20 DHT;

uint16_t temp = 73;

uint8_t count = 0;

unsigned long lastUIUpdate = 0;
const unsigned long UI_INTERVAL = 50; // The 50ms interval

uint32_t lcd_tickProvider()
{
  return millis();
}

#ifdef __cplusplus
extern "C"
{
#endif
  void log_to_serial(const char *message)
  {
    Serial.println(message);
    ESP.restart();
  }
#ifdef __cplusplus
}
#endif
long x, y, z;

// Get the Touchscreen data
void touchscreen_read(lv_indev_t *indev, lv_indev_data_t *data)
{
  // Checks if Touchscreen was touched, and prints X, Y and Pressure (Z)
  if (touchscreen.tirqTouched() && touchscreen.touched())
  {
    // Get Touchscreen points
    TS_Point p = touchscreen.getPoint();
    // Calibrate Touchscreen points with map function to the correct width and height
    x = map(p.x, 200, 3700, 1, TFT_HOR_RES);
    y = map(p.y, 240, 3800, 1, TFT_VER_RES);
    z = p.z;

    data->state = LV_INDEV_STATE_PRESSED;

    // Set the coordinates
    data->point.x = x;
    data->point.y = y;

    // Print Touchscreen info about X, Y and Pressure (Z) on the Serial Monitor
    // Serial.print("X = ");
    // Serial.print(x);
    // Serial.print(" | Y = ");
    // Serial.print(y);
    // Serial.print(" | Pressure = ");
    // Serial.print(z);
    // Serial.println();
  }
  else
  {
    data->state = LV_INDEV_STATE_RELEASED;
  }
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

  // touchscreenSPI.begin(XPT2046_CLK, XPT2046_MISO, XPT2046_MOSI, XPT2046_CS);
  touchscreen.begin();
  // Set the Touchscreen rotation in landscape mode
  // Note: in some displays, the touchscreen might be upside down, so you might need to set the rotation to 0: touchscreen.setRotation(0);
  touchscreen.setRotation(0);

  /*Set a tick source so that LVGL will know how much time elapsed. */
  lv_tick_set_cb(lcd_tickProvider);

  /*TFT_eSPI can be enabled lv_conf.h to initialize the display in a simple way*/
  lv_display_t *disp = lv_tft_espi_create(TFT_HOR_RES, TFT_VER_RES, lcd_drawBufffer, sizeof(lcd_drawBufffer));
  lv_display_set_rotation(disp, LV_DISPLAY_ROTATION_90);

  lv_indev_t *indev = lv_indev_create();
  lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);
  lv_indev_set_read_cb(indev, touchscreen_read);
  lv_indev_set_long_press_time(indev, 250);
  lv_indev_set_long_press_repeat_time(indev, 250);

  ui_init();
  Serial.println("Setup done");

  // lv_timer_create(lcd_updateCookTime, 1000, NULL); // Create a timer to update the temperature every second
}

void loop()
{

  //  boolean istouched = touchscreen.touched();
  // if (istouched) {
  //   Serial.println("TOUCHED");
  //   istouched = false;
  // }

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

  if (millis() - lastUIUpdate >= UI_INTERVAL)
  {
    lv_task_handler();
    lastUIUpdate = millis();
  }

  // if (millis() % 1000 < 10)
  // {
  //   // Update temperature display every second
  //   lv_label_set_text_fmt(ui_Temperature, "%d", temp++);
  // }
  // delay(5); // Add a small delay to allow for smoother updates
}

//  -- END OF FILE --