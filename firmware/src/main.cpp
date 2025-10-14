#include <Arduino.h>
#include "DHT20.h"
#include <lvgl.h>
#include <TFT_eSPI.h> // Hardware-specific library
#include "ui/ui.h"
#include <XPT2046_Touchscreen.h>
#include "sensor.h"

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

uint16_t setTemperature = 45;
float temperature = 0;
float humidity = 0;

unsigned long lastUIUpdate = 0;
const unsigned long UI_INTERVAL = 50; // The 50ms interval

long touch_x, touch_y, touch_z;

uint32_t lcd_tickProvider()
{
  return millis();
}

void incrementTemp(int8_t delta)
{
  // TODO Underflow issues with unsigned integer
  setTemperature += delta;
  if (setTemperature < 0)
  {
    setTemperature = 0;
  }
  if (setTemperature > 100)
  {
    setTemperature = 100;
  }
  lv_label_set_text_fmt(ui_Set_Temperature, "%d", setTemperature);
}

void touchscreen_read(lv_indev_t *indev, lv_indev_data_t *data)
{
  if (touchscreen.tirqTouched() && touchscreen.touched())
  {
    TS_Point p = touchscreen.getPoint();

    // Calibrate Touchscreen points with map function to the correct width and height
    touch_x = map(p.x, 200, 3700, 1, TFT_HOR_RES);
    touch_y = map(p.y, 240, 3800, 1, TFT_VER_RES);
    touch_z = p.z;

    data->state = LV_INDEV_STATE_PRESSED;
    data->point.x = touch_x;
    data->point.y = touch_y;
  }
  else
  {
    data->state = LV_INDEV_STATE_RELEASED;
  }
}

void setup()
{
  Serial.begin(115200);
  Serial.println("Thermostat Controller");

  Wire.begin();

#ifdef DHT_ENABLED
  sensor_setup(&humidity, &temperature);
#endif

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
  lv_indev_set_long_press_time(indev, 500);
  lv_indev_set_long_press_repeat_time(indev, 250);

  ui_init();
  Serial.println("Setup done");

  // lv_timer_create(lcd_updateCookTime, 1000, NULL); // Create a timer to update the temperature every second
}

void loop()
{

#ifdef DHT_ENABLED
  // TODO Temperature and humidity is only updated when 0 is returned
  int err = sensor_loop();
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
