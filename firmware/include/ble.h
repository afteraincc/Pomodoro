#ifndef _POMODORO_BLE_H_
#define _POMODORO_BLE_H_

#include <stdint.h>
#include <BLEDevice.h>
#include <BLEService.h>
#include <BLECharacteristic.h>

enum PomodoroStatus
{
    Working,
    Break,
    Idle
};

class PomodoroBLE
{
public:
    PomodoroBLE();

    void init();
    void notify(enum PomodoroStatus status, int32_t total, int32_t second);
    void advertising();

    void onClientConnect();
    void onClientDisconnect();
    void onWrite(BLECharacteristic *characteristic);

    bool clientConnected();

    int getWorkMinute();
    void setWorkMinute(int32_t m);

    int getBreakMinute();

    bool workMinuteIsUpdated();
    void updateWorkMinute();

private:
    BLEServer *server;
    BLEService *service;
    BLECharacteristic *statusCharacteristic;
    BLECharacteristic *minuteCharacteristic;
    bool connected;
    int32_t curWorkMinute;
    int32_t preWorkMinute;
    uint16_t notifyParam[3];
};

#endif