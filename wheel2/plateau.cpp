#include "log.h"
#include "plateau.h"
#include "pins.h"
#include "pwm.h"
#include "helper.h"

Plateau::Plateau(Shared& shared, SpeedComp& speedcomp) :
  _shared(shared),
  _speedcomp(speedcomp),
  _interval(5000, TM_MICROS),
  turnInterval(10, TM_MILLIS) {
} // Plateau()

void Plateau::init() {
  LOG_DEBUG("plateau.cpp", "[init]");

  pinMode(PLATEAU_A_PIN,  INPUT_PULLUP);
  pinMode(PLATEAU_B_PIN,  INPUT_PULLUP);
  pinMode(PLATEAU_EN_PIN, OUTPUT);

  setPwm(PLATEAU_MOTOR_P_PIN);
  setPwm(PLATEAU_MOTOR_N_PIN);
} // init()

void Plateau::func() {
  if (_interval.tick()) {
    _speedcomp.update();

    float speed = _speedcomp.speedCenterComp;

    if (motorOn) {
      _outBuff = pid(speed); // calculate motor power
      _outBuffPrev = _outBuff;
      _outBuff += _speedcomp.unbalanceComp;
      _outBuff = limitFloat(_outBuff, -100, 100);
      
      pwmPhase(_outBuff / 100.0, PLATEAU_MOTOR_N_PIN, PLATEAU_MOTOR_P_PIN);
    } else {
      pwmPhase(0, PLATEAU_MOTOR_P_PIN, PLATEAU_MOTOR_N_PIN);
      _basic = 0; // reset I
    }
    update();
  } // _interval.tick()
} // func()

void Plateau::update() {
  if (!logic || _shared.state == S_RECORD_CLEAN) {
    return;
  }
  float speed = _speedcomp.speed;

  if (motorOn) { // is motor on?
    if (_shared.state == S_BAD_ORIENTATION
      || _shared.state == S_HOME
      // || _shared.state == S_PARKING
      // || _shared.state == S_HOMING
      ) {
      // LOG_ALERT("plateau.cpp", "[update] Seems that plateau is still turning!!");
      Serial.println("PLATEAU: STILL TURNING!");
      stop();
      return;
    }

    if (atSpeed) { // atSpeed == true
      // if (speed > targetRpm * 4) { // too fast 200%
      //   LOG_ALERT("plateau.cpp", "[update] Too fast!!");
      //   Serial.println("PLATEAU: TOO FAST");
      //   stop();
      //   return;
      // }
      if (speed < targetRpm * 0.65 && turnInterval.duration() > 1500) { // too slow 65%
        // LOG_NOTICE("plateau.cpp", "[update] Stopped by hand");
        Serial.println("PLATEAU: STOPPED BY HAND");
        stop();
        return;
      }
    } else { // atSpeed == false
      if (speed > targetRpm * 0.95) { // at speed 95%
        // LOG_INFO("plateau.cpp", "[update] At speed");
        Serial.println("PLATEAU: AT SPEED");
        atSpeed = true;
        return;
      }
      if (speed < targetRpm * 0.1 && turnInterval.duration() > 750) { // <5% target speed after short time
        // LOG_ALERT("plateau.cpp", "[update] Could not speed up!!");
        Serial.println("PLATEAU: COULD NOT SPEED UP");
        stop();
        return;
      }
    }
  } else { // motorOn = false
    if (turnInterval.duration() > 1000 && !spinningDown) { // spinned by swing
      if (speed > PLATEAU_RPM33 * 0.666 && (_shared.state == S_HOME 
        || _shared.state == S_HOMING || _shared.state == S_PARKING)) { // 50% of 33.3 speed
        // LOG_INFO("plateau.cpp", "[update] Started by swing");
        Serial.println("PLATEAU: STARTED BY SWING");
        play();
        return;
      }
    }
    if (spinningDown && speed < PLATEAU_RPM33 * 0.05) { // <5% of 33.3 speed (spinning down)
      spinningDown = false;
      turnInterval.reset();
      // LOG_INFO("plateau.cpp", "[update] Stopped");
      Serial.println("PLATEAU: STOPPED");
      return;
    }
  }
} // update()

void Plateau::motorStart() {
  LOG_DEBUG("plateau.cpp", "[motorStart]");
  motorOn = true;
  setRpm(RPM_33);

  _basic = 30; // 50; //40; //60; //75;

  Serial.println("PLATEAU: ON");
} // motorStart

void Plateau::motorStop() {
  LOG_DEBUG("plateau.cpp", "[motorStop]");
  motorOn = false;
  targetRpm = 0;

  turnInterval.reset();
  spinningDown = true;
  atSpeed = false;

  Serial.println("PLATEAU: OFF");
} // motorStop

void Plateau::updateRpm() {
  LOG_DEBUG("plateau.cpp", "[updateRpm]");
  if (!motorOn) {
    return;
  }
  if (rpmMode == RPM_AUTO) {
    targetRpm = _autoRpm;
  } else if (rpmMode == RPM_33) {
    targetRpm = PLATEAU_RPM33;
  } else if (rpmMode == RPM_45) {
    targetRpm = PLATEAU_RPM45;
  } else if (rpmMode == RPM_78) {
    targetRpm = PLATEAU_RPM78;
  }
  LOG_DEBUG("plateau.cpp", "[updateRpm] targetRpm: " + String(targetRpm));
} // updateRpm

void Plateau::setRpm(eRpmMode rpmMode) {
  LOG_DEBUG("plateau.cpp", "[setRpm]");
  float rpm;
  if (rpmMode == RPM_AUTO) {
    rpm = PLATEAU_RPM33;
  } else if (rpmMode == RPM_33) {
    rpm = PLATEAU_RPM33;
  } else if (rpmMode == RPM_45) {
    rpm = PLATEAU_RPM45;
  } else if (rpmMode == RPM_78) {
    rpm = PLATEAU_RPM78;
  }

  if (rpm == targetRpm) {
    return;
  }
  _autoRpm = rpm;
  LOG_DEBUG("plateau.cpp", "[setRpm] autoRPM: " + String(_autoRpm));
  updateRpm();

  _speedcomp.clearCompSamples();
  turnInterval.reset();
} // setRpm

float Plateau::pid(float rpm) {
  float pp, pd;
  float diffRpm = rpm - _rpmPrev;
  _rpmPrev = rpm;
  float diffTargetRpm = targetRpm - rpm;

  if (unbalanceCompensation) {
    pp = diffTargetRpm * P;
    _basic += diffTargetRpm * I; // bring basic voltage back to average for proper speed
    _basic = limitFloat(_basic, 40, 80);
    pd = diffRpm * D;
  }
  return  pp + _basic + pd;
} // pid()

void Plateau::play() {
  LOG_DEBUG("plateau.cpp", "[play]");
  _shared.setState(S_HOMING_BEFORE_PLAYING); // Home first
  motorStart();
  if (_shared.puristMode) {
    _shared.puristMode = false;
    Serial.println("PURIST MODE: OFF");
  }
} // play()

void Plateau::stop() {
  LOG_DEBUG("plateau.cpp", "[stop]");
  if (_shared.state == S_HOMING_BEFORE_PLAYING) { // To prevent error triggers when stopping during homing
    _shared.state = S_HOMING; // Seems extreme, but otherwise state-change timer will change
  } else {
    _shared.setState(S_STOPPING);
  }
  motorStop();
  if (_shared.puristMode) {
    _shared.puristMode = false;
    Serial.println("PURIST MODE: OFF");
  }
} // stop()

void Plateau::cleanMode() {
  // LOG_DEBUG("plateau.cpp", "[cleanMode]");
  _shared.setState(S_HOMING_BEFORE_CLEANING);
} // cleanMode()

void Plateau::info() {
  int padR = 25;
  Serial.println(padRight("PLATEAU_P", padR) +          ": " + String(P, 5));
  Serial.println(padRight("PLATEAU_I", padR) +          ": " + String(I, 5));
  Serial.println(padRight("PLATEAU_D", padR) +          ": " + String(D, 5));
  Serial.println(padRight("PLATEAU_MOTOR", padR) +      ": " + String(motorOn ? "ON" : "OFF"));
  Serial.println(padRight("PLATEAU_RPM_MODE", padR) +   ": " + getRpmState(rpmMode));
  Serial.println(padRight("PLATEAU_TARGET_RPM", padR) + ": " + String(targetRpm, 2));
  Serial.println();
} // info()
