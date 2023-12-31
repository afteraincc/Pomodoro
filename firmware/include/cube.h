#ifndef _POMODORO_CUBE_H_
#define _POMODORO_CUBE_H_

#include <Arduino.h>

class Cube
{
public:
    void init();
    bool isSuccessed();

    bool isWorkFace();
    bool isIdleFace();
    bool isBreakFace();
    // dice
    // 1: front, 6: back
    // 2: left(work face), 5: right(break face)
    // 3: top(idle face), 4: bottom
    int currentFaceId();

    bool readYawPitchRoll();
    int getYaw();
    int getPitch();
    int getRoll();

private:
    // 0 = success
    // ERROR!
    // 1 = initial memory load failed
    // 2 = DMP configuration updates failed
    // (if it's going to break, usually the code will be 1)
    uint8_t devStatus;
    int ypr[3];
    int delta;
};

#endif