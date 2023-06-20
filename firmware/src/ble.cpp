#include "ble.h"
#include <BLE2902.h>
#include <Nvs.h>

#define POMODORO_BLE_SERVICE "221827F2-4CC5-483B-8DF0-866BD41BD2A2"
#define POMODORO_BLE_STATUS "22390B44-2557-4B2E-A467-70A18D02591E"
#define POMODORO_BLE_MINUTE "F7B560F0-6626-4A7B-B6D8-2D2C11F85441"

PomodoroBLE::PomodoroBLE()
    : server(NULL), service(NULL),
      statusCharacteristic(NULL), minuteCharacteristic(NULL),
      connected(false), curWorkMinute(25), preWorkMinute(25)
{
}

class PomodoroBLEServerCallbacks : public BLEServerCallbacks
{
public:
    PomodoroBLEServerCallbacks(PomodoroBLE *pomodoro) : BLEServerCallbacks()
    {
        this->pomodoro = pomodoro;
    }

    void onConnect(BLEServer *pServer)
    {
        pomodoro->onClientConnect();
    }

    void onDisconnect(BLEServer *pServer)
    {
        pomodoro->onClientDisconnect();
    }

private:
    PomodoroBLE *pomodoro;
};

class PomodoroBLECharacteristicCallbacks : public BLECharacteristicCallbacks
{
public:
    PomodoroBLECharacteristicCallbacks(PomodoroBLE *pomodoro) : BLECharacteristicCallbacks()
    {
        this->pomodoro = pomodoro;
    }

    void onWrite(BLECharacteristic *characteristic)
    {
        pomodoro->onWrite(characteristic);
    }

private:
    PomodoroBLE *pomodoro;
};

void PomodoroBLE::init()
{
    BLEDevice::init("Pomodoro");
    server = BLEDevice::createServer();
    server->setCallbacks(new PomodoroBLEServerCallbacks(this));
    service = server->createService(POMODORO_BLE_SERVICE);
    statusCharacteristic = service->createCharacteristic(POMODORO_BLE_STATUS, BLECharacteristic::PROPERTY_NOTIFY);
    statusCharacteristic->addDescriptor(new BLE2902());
    minuteCharacteristic = service->createCharacteristic(POMODORO_BLE_MINUTE, BLECharacteristic::PROPERTY_WRITE);
    minuteCharacteristic->setCallbacks(new PomodoroBLECharacteristicCallbacks(this));

    service->start();
    server->getAdvertising()->start();
}

void PomodoroBLE::advertising()
{
    server->getAdvertising()->start();
}

void PomodoroBLE::onClientConnect()
{
    connected = true;
}

void PomodoroBLE::onClientDisconnect()
{
    connected = false;
}

void PomodoroBLE::onWrite(BLECharacteristic *characteristic)
{
    std::string value;
    if (minuteCharacteristic == characteristic)
    {
        value = minuteCharacteristic->getValue();
        if (value.length() > 0)
        {
            curWorkMinute = atoi(value.c_str());
        }
    }
}

void PomodoroBLE::notify(enum PomodoroStatus status, int32_t total, int32_t second)
{
    if (connected)
    {
        memset(notifyParam, 0, sizeof(notifyParam));
        notifyParam[0] = status;
        notifyParam[1] = total;
        notifyParam[2] = second;
        statusCharacteristic->setValue((uint8_t *)&notifyParam, 6);
        statusCharacteristic->notify();
    }
}

bool PomodoroBLE::clientConnected()
{
    return connected;
}

int PomodoroBLE::getWorkMinute()
{
    return curWorkMinute;
}

void PomodoroBLE::setWorkMinute(int32_t m)
{
    curWorkMinute = m;
    preWorkMinute = m;
}

bool PomodoroBLE::workMinuteIsUpdated()
{
    return (preWorkMinute != curWorkMinute) ? true : false;
}

void PomodoroBLE::updateWorkMinute()
{
    setWorkMinute(curWorkMinute);
}

int PomodoroBLE::getBreakMinute()
{
    return 5;
}
