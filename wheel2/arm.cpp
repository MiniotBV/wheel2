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
      
      if (weight > _justInGroveWeight) { // is the needle already in the groove?
        weight = targetWeight; // put arm on target weight immediately
      } else if (weight < justDockedWeight) {
        weight = justDockedWeight;
      } else {
        weight += (_interval.interval / _speedUp) * (_justInGroveWeight - justDockedWeight);
      }
    } else { // should the motor be off?
      _motorOnInterval.reset();

      if (weight < justDockedWeight) { // is needle up?
        weight = ARM_DOCKED_WEIGHT; // turn off arm immediately
      } else if ( weight > _justInGroveWeight) {
        weight = _justInGroveWeight;
      } else {
        weight -= (_interval.interval / _speedDown) * (_justInGroveWeight - justDockedWeight);
      }
    }

    force = armWeight2Pwm(weight);
    pwmWriteFloat(ARM_MOTOR_PIN, force);

    if (weight != targetWeight) {
      _needleDownInterval.reset();
    }
  } // _interval.tick()
} // func()


bool Arm::putNeedleInGrove() {
  motorOn = true;
  return isNeedleInGrove();
} // putNeedleInGrove()


bool Arm::dockNeedle() {
  motorOn = false;
  return isNeedleDocked();
} // dockNeedle()


bool Arm::needleEmergencyStop() {
  LOG_DEBUG("arm.cpp", "[needleEmergencyStop]");
  weight = ARM_DOCKED_WEIGHT;
  motorOn = false;
  return true;
} // needleEmergencyStop()


bool Arm::isNeedleInGrove() {
  return weight == targetWeight;
} // isNeedleInGrove()


bool Arm::isNeedleDocked() {
  return weight == ARM_DOCKED_WEIGHT;
} // isNeedleDocked()


bool Arm::isNeedleDownFor(int ms) {
  return isNeedleInGrove() && _needleDownInterval.duration() > ms;
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
  Serial.println(padRight("ARM_FORCE_LOW", padR) +     ": " + String(forceLow,  5));
  Serial.println(padRight("ARM_FORCE_HIGH", padR) +    ": " + String(forceHigh, 5));
  Serial.println(padRight("ARM_FORCE", padR) +         ": " + String(force,  5));
  Serial.println(padRight("ARM_WEIGHT", padR) +        ": " + String(weight, 5));
  Serial.println(padRight("ARM_TARGET_WEIGHT", padR) + ": " + String(targetWeight, 5));
  Serial.println(padRight("ARM_MOTOR", padR) +         ": " + String(motorOn ? "ON" : "OFF"));
  Serial.println(padRight("NEEDLE", padR) +            ": " + String(isNeedleInGrove() ? "DOWN" : "UP"));
  Serial.println();
} // info()
