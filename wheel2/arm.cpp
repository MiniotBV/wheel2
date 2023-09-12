#include "log.h"
#include "arm.h"
#include "helper.h"
#include "pins.h"
#include "pwm.h"

Arm::Arm(Shared& shared) :
  _shared(shared),
  _interval(10, TM_MILLIS),
  _needleDownInterval(0, TM_MILLIS),
  _motorOnInterval(0, TM_MILLIS),
  _motorOffInterval(0, TM_MILLIS) {
} // Arm()

void Arm::init() {
  LOG_DEBUG("arm.cpp", "[init]");
  setPwm(ARM_MOTOR_PIN);

  armAngleRaw = analogRead(ARM_ANGLE_SENSOR_PIN);
  armAngleSlow = armAngleRaw;
} // init()

void Arm::func() {
  if (_interval.tick()) {
    if (_shared.state == S_CALIBRATE) {
      weight = pwm2ArmWeight(force);
      pwmWriteFloat(ARM_MOTOR_PIN, force);
      return;
    }

    if (_shared.state == S_HOMING_BEFORE_PLAYING
      || _shared.state == S_HOMING_BEFORE_CLEANING
      || _shared.state == S_HOMING) {
      if (motorOn) { // motorOn == true
        // LOG_CRITICAL("arm.cpp", "[func] Needle should not have been turned on!");
        Serial.println("Needle should not have been turned on!");
        needleEmergencyStop();
      }
    }

    if (motorOn) { // should the motor be on?
      _motorOffInterval.reset();
      
      if (weight > _justDownWeight - 0.01) { // is the needle down? (-0.01gr to prevent glitching)
        weight = targetWeight; // put arm on target weight immediately
      } else {
        weight = mapFloat(_motorOnInterval.duration(), 0, _speedUp, justHomeWeight, _justDownWeight);
      }
    } else { // should the motor be off?
      _motorOnInterval.reset();

      if (weight < justHomeWeight) { // is needle up?
        weight = ARM_HOME_WEIGHT; // turn off arm immediately
      } else {
        weight = mapFloat(_motorOffInterval.duration(), 0, _speedDown, _justDownWeight, justHomeWeight);
      }
    }

    force = armWeight2Pwm(weight);
    pwmWriteFloat(ARM_MOTOR_PIN, force);

    if (weight != targetWeight) {
      _needleDownInterval.reset();
    }
  } // _interval.tick()
} // func()

bool Arm::needleDown() {
  motorOn = true;
  return isNeedleDown();
} // needleDown()

bool Arm::needleUp() {
  motorOn = false;
  return isNeedleUp();
} // needleUp()

bool Arm::needleEmergencyStop() {
  LOG_DEBUG("arm.cpp", "[needleEmergencyStop]");
  weight = ARM_HOME_WEIGHT;
  motorOn = false;
  return true;
} // needleEmergencyStop()

bool Arm::isNeedleDown() {
  return weight == targetWeight;
} // isNeedleDown()

bool Arm::isNeedleUp() {
  return weight == ARM_HOME_WEIGHT;
} // isNeedleUp()

bool Arm::isNeedleDownFor(int ms) {
  return isNeedleDown() && _needleDownInterval.duration() > ms;
} // isNeedleDownFor()

void Arm::centerArmAngle() {
  // LOG_DEBUG("arm.cpp", "[centerArmAngle]");
  armAngleOffset = armAngleSlow;
} // centerArmAngle

void Arm::calibrateAngle() {
  LOG_DEBUG("arm.cpp", "[calibrateAngle]");
  armAngleMin = armAngleMinCall;
  armAngleMax = armAngleMaxCall;

  armAngleMinCall = ARM_AMAX;
  armAngleMaxCall = 0;
  // LOG_DEBUG("arm.cpp", "[calibrateAngle] ArmAngle calibrated and buffer values reset. MIN:" + String(armAngleMin) + " MAX:" + String(armAngleMax));
  Serial.println("ArmAngle calibrated and buffer values reset. MIN:" + String(armAngleMin) + " MAX:" + String(armAngleMax));
} // calibrateAngle()

float Arm::armWeight2Pwm(float weight) {
  float pwm = mapFloat(weight, ARM_MIN_WEIGHT, ARM_MAX_WEIGHT, forceLow, forceHigh);
  return limitFloat(pwm, 0, 1);
} // armWeight2Pwm()

float Arm::pwm2ArmWeight(float pwm) {
  return mapFloat(pwm, forceLow, forceHigh, ARM_MIN_WEIGHT, ARM_MAX_WEIGHT);
} // pwm2ArmWeight()

void Arm::info() {
  int padR = 25;
  Serial.println(padRight("ARM_FORCE_LOW", padR) +  ": " + String(forceLow,  5));
  Serial.println(padRight("ARM_FORCE_HIGH", padR) + ": " + String(forceHigh, 5));
  Serial.println(padRight("ARM_FORCE", padR) +  ": " + String(force,  5));
  Serial.println(padRight("ARM_WEIGHT", padR) + ": " + String(weight, 5));
  Serial.println(padRight("ARM_TARGET_WEIGHT", padR) + ": " + String(targetWeight, 5));
  Serial.println(padRight("ARM_MOTOR", padR) + ": " + String(motorOn ? "ON" : "OFF"));
  Serial.println(padRight("NEEDLE", padR) + ": " + String(isNeedleDown() ? "DOWN" : "UP"));
  Serial.println();
} // info()
