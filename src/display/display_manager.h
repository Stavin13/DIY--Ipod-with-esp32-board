#ifndef DISPLAY_MANAGER_H
#define DISPLAY_MANAGER_H

#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <SPI.h>
#include <lvgl.h>
#include "../config.h"

class DisplayManager {
private:
    Adafruit_ILI9341 tft;
    static lv_disp_draw_buf_t draw_buf;
    static lv_color_t buf[TFT_WIDTH * 10];
    static lv_disp_drv_t disp_drv;
    
    static void disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p);
    
public:
    DisplayManager();
    bool init();
    void setBrightness(uint8_t brightness);
    void sleep();
    void wake();
    Adafruit_ILI9341* getTFT() { return &tft; }
};

#endif // DISPLAY_MANAGER_H