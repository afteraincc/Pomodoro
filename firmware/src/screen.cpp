#include "screen.h"

Screen::Screen()
    : buf(NULL)
{
}

static void disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p)
{
    TFT_eSPI *tft = (TFT_eSPI *)disp->user_data;

    uint32_t w = (area->x2 - area->x1 + 1);
    uint32_t h = (area->y2 - area->y1 + 1);

    tft->startWrite();
    tft->setAddrWindow(area->x1, area->y1, w, h);
    tft->pushColors((uint16_t *)&color_p->full, w * h, true);
    tft->endWrite();

    lv_disp_flush_ready(disp);
}

void Screen::init(uint32_t w, uint32_t h)
{
    lv_init();

    tft = new TFT_eSPI(w, h);
    tft->init();

    buf = (lv_color_t *)malloc(sizeof(lv_color_t) * w * h / 10);
    lv_disp_draw_buf_init(&draw_buf, buf, NULL, w * h / 10);

    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = w;
    disp_drv.ver_res = h;
    disp_drv.flush_cb = disp_flush;
    disp_drv.draw_buf = &draw_buf;
    disp_drv.user_data = tft;
    lv_disp_drv_register(&disp_drv);
}

void Screen::rotate(lv_disp_rot_t rotation)
{
    switch (rotation)
    {
    case LV_DISP_ROT_NONE:
        tft->setRotation(0);
        lv_disp_set_rotation(NULL, rotation);
        break;
    case LV_DISP_ROT_90:
        tft->setRotation(1);
        lv_disp_set_rotation(NULL, rotation);
        break;
    case LV_DISP_ROT_270:
        tft->setRotation(3);
        lv_disp_set_rotation(NULL, rotation);
        break;
    }
}