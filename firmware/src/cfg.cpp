#include "cfg.h"
#include <nvs.h>

PomodoroCfgClass PomodoroCfg;

void PomodoroCfgClass::loadCfg()
{
    nvs_handle_t handle;
    int32_t cfgWorkMinute = 0;

    nvs_open("PomodoroBLE", NVS_READWRITE, &handle);
    nvs_get_i32(handle, "Minute", &cfgWorkMinute);
    nvs_commit(handle);
    nvs_close(handle);

    if (0 == cfgWorkMinute)
    {
        cfgWorkMinute = 25;
    }
    workMinute = cfgWorkMinute;
}

void PomodoroCfgClass::saveCfg()
{
    nvs_handle_t handle;
    nvs_open("PomodoroBLE", NVS_READWRITE, &handle);
    nvs_set_i32(handle, "Minute", workMinute);
    nvs_commit(handle);
    nvs_close(handle);
}

int PomodoroCfgClass::getWorkMinute()
{
    return workMinute;
}

void PomodoroCfgClass::setWorkMinute(int32_t m)
{
    workMinute = m;
}