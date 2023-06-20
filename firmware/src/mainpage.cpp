#include "mainpage.h"
#include <Arduino.h>

MainPage::MainPage()
    : connectionLed(NULL), workLed(NULL),
      breakLed(NULL), timerLabel(NULL)
{
}

void MainPage::init()
{
    lv_obj_t *connectionLabel = lv_label_create(lv_scr_act());
    lv_obj_align(connectionLabel, LV_ALIGN_TOP_MID, -50, 5);
    lv_label_set_text(connectionLabel, "Conn");
    connectionLed = lv_led_create(lv_scr_act());
    lv_obj_align(connectionLed, LV_ALIGN_TOP_MID, -50, 25);
    lv_led_set_color(connectionLed, lv_color_hex(0x00ff00));
    lv_led_off(connectionLed);

    lv_obj_t *workingLabel = lv_label_create(lv_scr_act());
    lv_obj_align(workingLabel, LV_ALIGN_TOP_MID, 0, 5);
    lv_label_set_text(workingLabel, "Work");
    workLed = lv_led_create(lv_scr_act());
    lv_obj_align(workLed, LV_ALIGN_TOP_MID, 0, 25);
    lv_led_set_color(workLed, lv_color_hex(0xff0000));
    lv_led_off(workLed);

    lv_obj_t *breakLabel = lv_label_create(lv_scr_act());
    lv_obj_align(breakLabel, LV_ALIGN_TOP_MID, 50, 5);
    lv_label_set_text(breakLabel, "Break");
    breakLed = lv_led_create(lv_scr_act());
    lv_obj_align(breakLed, LV_ALIGN_TOP_MID, 50, 25);
    lv_led_set_color(breakLed, lv_color_hex(0x0000ff0));
    lv_led_off(breakLed);

    timerLabel = lv_label_create(lv_scr_act());
    lv_obj_align(timerLabel, LV_ALIGN_CENTER, 0, 0);
    lv_label_set_text(timerLabel, "00:00");
    lv_obj_set_style_text_font(timerLabel, &lv_font_montserrat_32, 0);

    yawPitchRollLabel = lv_label_create(lv_scr_act());
    lv_obj_align(yawPitchRollLabel, LV_ALIGN_TOP_MID, 0, 200);
    lv_label_set_text(yawPitchRollLabel, "Yaw:00 Pitch:00 Roll:00");
}

void MainPage::connectOn()
{
    lv_led_on(connectionLed);
}

void MainPage::connectOff()
{
    lv_led_off(connectionLed);
}

void MainPage::workOn()
{
    lv_led_on(workLed);
}

void MainPage::workOff()
{
    lv_led_off(workLed);
}

void MainPage::updateWorkProgress(uint32_t total, uint32_t second)
{
    lv_label_set_text_fmt(timerLabel, "%02d:%02d", (total - second) / 60, (total - second) % 60);
}

void MainPage::breakOn()
{
    lv_led_on(breakLed);
}

void MainPage::breakOff()
{
    lv_led_off(breakLed);
}

void MainPage::updateBreakProgress(uint32_t total, uint32_t second)
{
    updateWorkProgress(total, second);
}

void MainPage::updateYawPitchRoll(int yaw, int pitch, int roll)
{
    lv_label_set_text_fmt(yawPitchRollLabel, "Yaw:%02d Pitch:%02d Roll:%02d", yaw, pitch, roll);
}