#include <Arduino.h>
#include <Esp.h>
#include <Ticker.h>
#include "ble.h"
#include "cfg.h"
#include "screen.h"
#include "mainpage.h"
#include "loopticker.h"
#include "cube.h"

static Screen screen;
static MainPage mainPage;

static PomodoroBLE *pomodoroBLE = NULL;
static PomodoroStatus pomodoroStatus = PomodoroStatus::Idle;
static bool isAutoBreak = false;

static Ticker timer;
static uint32_t secondNum = 0;
static uint32_t timeoutRetryCount = 3; // 超时通知重试次数(秒数)，确保客户端接收到结束消息

// longIntervalSecond: 未连接时，BLE定时广播
static LoopTicker loopTicker(5, 10, 1);

static Cube cube;

void echoSystemInfo()
{
  Serial.printf("chip revision: %d\n", ESP.getChipRevision());
  Serial.printf("chip model: %s\n", ESP.getChipModel());
  Serial.printf("chip cores: %d\n", ESP.getChipCores());
  Serial.printf("cpu FreqMHz: %d\n", ESP.getCpuFreqMHz());
  Serial.printf("sdk version: %s\n", ESP.getSdkVersion());

  Serial.printf("heap size: %d\n", ESP.getHeapSize());
  Serial.printf("free heap: %d\n", ESP.getFreeHeap());
  Serial.printf("mini free heap: %d\n", ESP.getMinFreeHeap());
  Serial.printf("max alloc heap: %d\n", ESP.getMaxAllocHeap());

  Serial.printf("psram size: %d\n", ESP.getPsramSize());
  Serial.printf("free psram: %d\n", ESP.getFreePsram());
  Serial.printf("mini free psram: %d\n", ESP.getMinFreePsram());
  Serial.printf("max alloc psram: %d\n", ESP.getMaxAllocPsram());

  Serial.printf("flash size: %d\n", ESP.getFlashChipSize());
  Serial.printf("flash speed: %d\n", ESP.getFlashChipSpeed());
  Serial.printf("flash mode: %d\n", ESP.getFlashChipMode());
}

void initScreen()
{
  screen.init(240, 240);
  mainPage.init();
  lv_timer_handler();
}

void initBLE()
{
  PomodoroCfg.loadCfg();

  pomodoroBLE = new PomodoroBLE();
  pomodoroBLE->setWorkMinute(PomodoroCfg.getWorkMinute());
  pomodoroBLE->init();
  Serial.printf("PomodoroBLE work minute: %d\n", pomodoroBLE->getWorkMinute());

  mainPage.updateWorkProgress(pomodoroBLE->getWorkMinute() * 60, 0);
}

void setup()
{
  Serial.begin(115200);
  echoSystemInfo();

  initScreen();
  initBLE();

  cube.init();
}

bool workTriggerOn()
{
  return !digitalRead(0x9);
}

bool cancelTimer()
{
  if (!timer.active())
    return false;

  timer.detach();
  return true;
}

void breakTriggerHandle();
void IRAM_ATTR onWorkTimer()
{
  bool timeout = false;
  bool timeoutRetrying = false;

  secondNum++;
  if (secondNum > pomodoroBLE->getWorkMinute() * 60)
    timeoutRetrying = true;
  if (secondNum > pomodoroBLE->getWorkMinute() * 60 + timeoutRetryCount)
    timeout = true;

  if (timeout)
  {
    isAutoBreak = true;
    breakTriggerHandle();
  }
  else
  {
    Serial.printf("PomodoroBLE::notify clientConnected: %d, status %d, total %d, second %d\n",
                  pomodoroBLE->clientConnected(), PomodoroStatus::Working, pomodoroBLE->getWorkMinute() * 60, secondNum);
    pomodoroBLE->notify(PomodoroStatus::Working, pomodoroBLE->getWorkMinute() * 60, secondNum);
    if (!timeoutRetrying)
      mainPage.updateWorkProgress(pomodoroBLE->getWorkMinute() * 60, secondNum);
    else
    {
      mainPage.updateWorkProgress(pomodoroBLE->getWorkMinute() * 60, 0);
      mainPage.workOff();
    }
  }
}

void IRAM_ATTR onBreakTimer()
{
  bool timeout = false;
  bool timeoutRetrying = false;

  secondNum++;
  if (secondNum > pomodoroBLE->getBreakMinute() * 60)
    timeoutRetrying = true;
  if (secondNum > pomodoroBLE->getBreakMinute() * 60 + timeoutRetryCount)
    timeout = true;

  if (timeout)
    cancelTimer();
  else
  {
    Serial.printf("PomodoroBLE::notify clientConnected: %d, status %d, total %d, second %d\n",
                  pomodoroBLE->clientConnected(), PomodoroStatus::Break, pomodoroBLE->getBreakMinute() * 60, secondNum);
    pomodoroBLE->notify(PomodoroStatus::Break, pomodoroBLE->getBreakMinute() * 60, secondNum);
    if (!timeoutRetrying)
      mainPage.updateBreakProgress(pomodoroBLE->getBreakMinute() * 60, secondNum);
    else
    {
      mainPage.updateWorkProgress(pomodoroBLE->getBreakMinute() * 60, 0);
      mainPage.breakOff();
    }
  }
}

void IRAM_ATTR onIdleTimer()
{
  bool timeout = false;
  int idleSeconds = 3;

  secondNum++;
  if (secondNum > idleSeconds)
    timeout = true;

  if (timeout)
  {
    mainPage.updateWorkProgress(pomodoroBLE->getWorkMinute() * 60, 0);
    cancelTimer();
  }
  else
  {
    Serial.printf("PomodoroBLE::notify clientConnected: %d, status %d, total %d, second %d\n",
                  pomodoroBLE->clientConnected(), PomodoroStatus::Idle, idleSeconds, secondNum);
    pomodoroBLE->notify(PomodoroStatus::Idle, idleSeconds, secondNum);
    mainPage.updateBreakProgress(idleSeconds, secondNum);
  }
}

bool startWorkTimer()
{
  if (PomodoroStatus::Working == pomodoroStatus)
    return false;
  if (isAutoBreak)
    return false;

  if (timer.active())
    cancelTimer();

  secondNum = 0;

  timer.attach(1, onWorkTimer);
  pomodoroStatus = PomodoroStatus::Working;
  return true;
}

bool startBreakTimer()
{
  if (PomodoroStatus::Break == pomodoroStatus)
    return false;

  if (timer.active())
    cancelTimer();

  secondNum = 0;

  timer.attach(1, onBreakTimer);
  pomodoroStatus = PomodoroStatus::Break;
  return true;
}

bool startIdleTimer()
{
  if (PomodoroStatus::Idle == pomodoroStatus)
    return false;

  if (timer.active())
    cancelTimer();

  secondNum = 0;

  timer.attach(1, onIdleTimer);
  pomodoroStatus = PomodoroStatus::Idle;
  return true;
}

void pomodoroBLEAdvertising()
{
  if (pomodoroBLE->clientConnected())
    return;

  Serial.printf("BLE advertising\n");
  pomodoroBLE->advertising();
}

void updateWorkMinute()
{
  if (!pomodoroBLE->workMinuteIsUpdated())
    return;

  PomodoroCfg.setWorkMinute(pomodoroBLE->getWorkMinute());
  PomodoroCfg.saveCfg();
  pomodoroBLE->updateWorkMinute();

  mainPage.updateWorkProgress(pomodoroBLE->getWorkMinute() * 60, 0);
}

void updateConnectStatus()
{
  if (pomodoroBLE->clientConnected())
    mainPage.connectOn();
  else
  {
    mainPage.updateWorkProgress(pomodoroBLE->getWorkMinute() * 60, 0);
    mainPage.workOff();
    mainPage.breakOff();
    mainPage.connectOff();

    cancelTimer();
  }
}

void workTriggerHandle()
{
  if (startWorkTimer())
  {
    Serial.printf("isWorkFace\n");
    mainPage.breakOff();
    mainPage.workOn();
    mainPage.updateWorkProgress(pomodoroBLE->getWorkMinute() * 60, 0);
    screen.rotate(LV_DISP_ROT_270);
  }
}

void breakTriggerHandle()
{
  if (startBreakTimer())
  {
    Serial.printf("isBreakFace\n");
    mainPage.workOff();
    mainPage.breakOn();
    mainPage.updateBreakProgress(pomodoroBLE->getBreakMinute() * 60, 0);
    if (!isAutoBreak)
      screen.rotate(LV_DISP_ROT_90);
  }
}

void idleTriggerHandle()
{
  if (startIdleTimer())
  {
    Serial.printf("isIdleFace\n");
    mainPage.breakOff();
    mainPage.workOff();
    mainPage.updateWorkProgress(pomodoroBLE->getWorkMinute() * 60, 0);
    screen.rotate(LV_DISP_ROT_NONE);
  }
}

void triggerHandle()
{
  if (!cube.readYawPitchRoll())
    return;
  mainPage.updateYawPitchRoll(cube.getYaw(), cube.getPitch(), cube.getRoll());

  if (!pomodoroBLE->clientConnected())
    return;

  if (cube.isWorkFace() || workTriggerOn())
    workTriggerHandle();
  else if (cube.isBreakFace())
  {
    isAutoBreak = false;
    breakTriggerHandle();
  }
  else if (cube.isIdleFace())
  {
    isAutoBreak = false;
    idleTriggerHandle();
  }
}

void loop()
{
  lv_timer_handler();
  delay(loopTicker.getTickMilliSecond());

  loopTicker.tick();
  if (loopTicker.longIntervalArrived())
  {
    loopTicker.resetLongInterval();

    pomodoroBLEAdvertising();
  }

  if (loopTicker.shortIntervalArrived())
  {
    loopTicker.resetShortInterval();

    updateWorkMinute();
    updateConnectStatus();
    triggerHandle();
  }
}