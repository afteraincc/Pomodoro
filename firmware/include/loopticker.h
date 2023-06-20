#ifndef _LOOPTICKER_H_
#define _LOOPTICKER_H_

#include <Arduino.h>

class LoopTicker
{
public:
    LoopTicker(uint32_t tickMilliSecond, uint32_t longIntervalSecond, uint32_t shortIntervalSecond);

    uint32_t getTickMilliSecond();
    void tick();
    bool longIntervalArrived();
    void resetLongInterval();
    bool shortIntervalArrived();
    void resetShortInterval();

private:
    uint32_t tickMS;

    uint32_t longInterval;
    uint32_t curLongTickNum;
    uint32_t shortInterval;
    uint32_t curShortTickNum;
};

#endif