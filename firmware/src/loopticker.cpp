#include "loopticker.h"

LoopTicker::LoopTicker(uint32_t tickMilliSecond, uint32_t longIntervalSecond, uint32_t shortIntervalSecond)
    : curLongTickNum(0), curShortTickNum(0)
{
    tickMS = tickMilliSecond;
    longInterval = longIntervalSecond;
    shortInterval = shortIntervalSecond;
}

uint32_t LoopTicker::getTickMilliSecond()
{
    return tickMS;
}

void LoopTicker::tick()
{
    curLongTickNum += tickMS;
    curShortTickNum += tickMS;
}

bool LoopTicker::longIntervalArrived()
{
    return curLongTickNum / 1000 >= longInterval;
}

void LoopTicker::resetLongInterval()
{
    curLongTickNum = 0;
}

bool LoopTicker::shortIntervalArrived()
{
    return curShortTickNum / 1000 >= shortInterval;
}

void LoopTicker::resetShortInterval()
{
    curShortTickNum = 0;
}