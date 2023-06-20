#ifndef _MAINPAGE_H_
#define _MAINPAGE_H_

#include "lv_conf.h"
#include <lvgl.h>

class MainPage
{
public:
    MainPage();
    void init();

    void connectOn();
    void connectOff();

    void workOn();
    void workOff();

    void breakOn();
    void breakOff();

    void updateWorkProgress(uint32_t total, uint32_t second);
    void updateBreakProgress(uint32_t total, uint32_t second);

    void updateYawPitchRoll(int yaw, int pitch, int roll);

private:
    lv_obj_t *connectionLed;
    lv_obj_t *workLed;
    lv_obj_t *breakLed;
    lv_obj_t *timerLabel;
    lv_obj_t *yawPitchRollLabel;
};

#endif