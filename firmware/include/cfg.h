#ifndef _POMODORO_CFG_H_
#define _POMODORO_CFG_H_

#include <stdint.h>

class PomodoroCfgClass
{
public:
    void loadCfg();
    void saveCfg();

    int getWorkMinute();
    void setWorkMinute(int32_t m);

private:
    int32_t workMinute;
};

extern PomodoroCfgClass PomodoroCfg;

#endif