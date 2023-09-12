#ifndef PLATEAU_H
#define PLATEAU_H

#include <Arduino.h>
#include "interval.h"
#include "shared.h"
#include "speedcomp.h"
#include "enums.h"

#define PLATEAU_RPM33 33.333
#define PLATEAU_RPM45 45
#define PLATEAU_RPM78 78

#define PLATEAU_ENABLE_RPM78 false

class Plateau {
  private:
    Interval _interval;
    Shared& _shared;
    SpeedComp& _speedcomp;
    float _autoRpm;
    float _basic = 0;
    float _outBuff;
    float _outBuffPrev;
    float _rpmPrev = 0;
    bool spinningDown;
    float pid(float rpmIn);
    void update();
  public:
    Interval turnInterval;
    float P = 1;    // 0.5
    float I = 0.02; // plateau33I;
    float D = 0;
    float targetRpm = 0;
    eRpmMode rpmMode = RPM_AUTO;
    bool logic = true;
    bool unbalanceCompensation = true;
    bool atSpeed;
    bool motorOn = false;
    Plateau(Shared& shared, SpeedComp& speedcomp);
    void init();
    void func();
    void motorStart();
    void updateRpm();
    void setRpm(eRpmMode rpm);
    void motorStop();
    void play();
    void stop();
    void cleanMode();
    void info();
}; // Plateau

#endif
