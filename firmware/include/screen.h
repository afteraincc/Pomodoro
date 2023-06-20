#ifndef _SCREEN_H_
#define _SCREEN_H_

#include "tft_setup.h"
#include <TFT_eSPI.h>
#include "lv_conf.h"
#include <lvgl.h>

class Screen
{
public:
    Screen();
    void init(uint32_t w, uint32_t h);
    void rotate(lv_disp_rot_t rotation);

private:
    TFT_eSPI *tft;
    lv_disp_drv_t disp_drv;
    lv_disp_draw_buf_t draw_buf;
    lv_color_t *buf;
};

#endif