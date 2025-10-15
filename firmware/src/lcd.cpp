#include "configuration.h"

#ifdef LCD_ENABLED
#include <Arduino.h>
#include <lvgl.h>
#include <TFT_eSPI.h> // Hardware-specific library
#include "ui/ui.h"
#include <XPT2046_Touchscreen.h>

XPT2046_Touchscreen touchscreen(TOUCH_CS, TOUCH_IRQ);

uint32_t lcd_drawBufffer[DRAW_BUF_SIZE / 4];

long touch_x, touch_y, touch_z;

unsigned long lastUIUpdate = 0;

uint32_t lcd_tickProvider()
{
    return millis();
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

void lcd_setup()
{
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

void lcd_loop()
{
    if (millis() - lastUIUpdate >= UI_UPDATE_INTERVAL)
    {
        lv_task_handler();

        // lv_label_set_text_fmt(ui_Temperature, "%.1f\xB0C", temperature);
        // lv_label_set_text_fmt(ui_Humidity, "%.1f%%", humidity);

        DeviceConfig *config = currentDeviceConfig();
        if (config->dirty)
        {
            lv_label_set_text_fmt(ui_SetTemperature, "%i\xB0C", config->setTemp);
            lv_obj_set_style_opa(ui_UpArrow, config->heatEnabled ? LV_OPA_COVER : LV_OPA_30, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_opa(ui_UpDivider, config->heatEnabled ? LV_OPA_COVER : LV_OPA_30, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_opa(ui_SetTemperature, config->heatEnabled ? LV_OPA_COVER : LV_OPA_30, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_opa(ui_SetTemperature_F, config->heatEnabled ? LV_OPA_COVER : LV_OPA_30, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_opa(ui_DownDivider, config->heatEnabled ? LV_OPA_COVER : LV_OPA_30, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_opa(ui_DownArrow, config->heatEnabled ? LV_OPA_COVER : LV_OPA_30, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        lastUIUpdate = millis();
    }
}
#endif