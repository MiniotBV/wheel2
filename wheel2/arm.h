#ifndef ARM_H
#define ARM_H

#include <Arduino.h>
#include "interval.h"
#include "shared.h"

#define ARM_MIN_WEIGHT  0.5  // in grams
#define ARM_MAX_WEIGHT  4    // in grams
#define ARM_HOME_WEIGHT -10

#define ARM_AMAX 4095

class Arm {
  private:
    Interval _interval;
    Interval _needleDownInterval;
    Interval _motorOnInterval;
    Interval _motorOffInterval;
    Shared& _shared;
    float _speedUp = 1000; //ms;
    float _speedDown = 500; //ms;
    float _justDownWeight = 0.5; //0.25;
    float armWeight2Pwm(float weight);
    float pwm2ArmWeight(float pwm);
  public:
    bool motorOn = false;
    float targetWeight = 2.1;
    float weight = ARM_HOME_WEIGHT;
    float force = 0;
    float forceLow = 0.3;  //0.13; //0.08;
    float forceHigh = 0.8; //0.13; //0.08;
    float justHomeWeight = -0.75;
    float armAngleMin = 1200;
    float armAngleMax = 3300;
    float armAngleMinCall = ARM_AMAX;
    float armAngleMaxCall = 0;
    float armAngleCall;
    float armAngleRaw;
    float armAnglePrev;
    float armAngleDiff;
    float armAngle;
    float armAngleSlow;
    float armAngleOffset;
    Arm(Shared& shared);
    void init();
    void func();
    bool needleDown();
    bool needleUp();
    bool needleEmergencyStop();
    bool isNeedleDown();
    bool isNeedleUp();
    bool isNeedleDownFor(int ms);
    void centerArmAngle();
    void calibrateAngle();
    void info();
}; // Arm

#endif
