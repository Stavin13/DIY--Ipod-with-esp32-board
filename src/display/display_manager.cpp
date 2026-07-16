#include "display_manager.h"

// Static member definitions
lv_disp_draw_buf_t DisplayManager::draw_buf;
lv_color_t DisplayManager::buf[TFT_WIDTH * 10];
lv_disp_drv_t DisplayManager::disp_drv;

DisplayManager::DisplayManager() : tft(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST, TFT_MISO) {
}

bool DisplayManager::init() {
    // Initialize TFT with Adafruit library
    tft.begin();
    tft.setRotation(0); // Portrait mode
    tft.fillScreen(ILI9341_BLACK);
    
    // Initialize LVGL
    lv_init();
    
    // Initialize display buffer
    lv_disp_draw_buf_init(&draw_buf, buf, NULL, TFT_WIDTH * 10);
    
    // Initialize display driver
    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = TFT_WIDTH;
    disp_drv.ver_res = TFT_HEIGHT;
    disp_drv.flush_cb = disp_flush;
    disp_drv.draw_buf = &draw_buf;
    disp_drv.user_data = &tft;
    lv_disp_drv_register(&disp_drv);
    
    Serial.println("Display initialized successfully");
    return true;
}

void DisplayManager::disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p) {
    uint32_t w = (area->x2 - area->x1 + 1);
    uint32_t h = (area->y2 - area->y1 + 1);
    
    Adafruit_ILI9341* tft_ptr = (Adafruit_ILI9341*)disp->user_data;
    if (tft_ptr) {
        // Convert LVGL colors to RGB565 and draw
        tft_ptr->startWrite();
        tft_ptr->setAddrWindow(area->x1, area->y1, w, h);
        
        for(uint32_t i = 0; i < w * h; i++) {
            tft_ptr->writePixel(0, 0, color_p[i].full); // This needs proper implementation
        }
        
        tft_ptr->endWrite();
    }
    
    lv_disp_flush_ready(disp);
}

void DisplayManager::setBrightness(uint8_t brightness) {
    // Implement PWM brightness control if needed
    // For now, just store the value
}

void DisplayManager::sleep() {
    // Put display to sleep - implementation depends on display
}

void DisplayManager::wake() {
    // Wake display - implementation depends on display
}