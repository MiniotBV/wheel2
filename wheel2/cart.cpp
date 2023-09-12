#include "log.h"
#include "cart.h"
#include "helper.h"
#include "pins.h"
#include "pwm.h"

Cart::Cart(Shared& shared, Arm& arm, Plateau& plateau, Scanner& scanner) :
  _shared(shared),
  _arm(arm),
  _plateau(plateau),
  _scanner(scanner),
  _interval(1000, TM_MICROS),
  movedForwardInterval(1, TM_MILLIS) {
} // Cart()

void Cart::init(SpeedComp* speedcomp) { // to prevent circular reference
  LOG_DEBUG("cart.cpp", "[init]");
  _speedcomp = speedcomp;
  setPwm(CART_STEPPER_AP_PIN);
  setPwm(CART_STEPPER_AN_PIN);
  setPwm(CART_STEPPER_BP_PIN);
  setPwm(CART_STEPPER_BN_PIN);
} // init()

void Cart::func() {
  if (_interval.tick()) {
    //--------------------------------------------- ARM ANGLE
    _arm.armAngleRaw += (analogRead(ARM_ANGLE_SENSOR_PIN) - _arm.armAngleRaw ) / 6;

    if (millis() > 1000) {
        if (_arm.armAngleRaw < _arm.armAngleMinCall) {
          _arm.armAngleMinCall = _arm.armAngleRaw;
        }
        if (_arm.armAngleRaw > _arm.armAngleMaxCall) {
          _arm.armAngleMaxCall = _arm.armAngleRaw;
        }
    }
    _arm.armAngleCall = mapFloat(_arm.armAngleRaw, _arm.armAngleMin, _arm.armAngleMax, 1, -1);
    _arm.armAngleDiff = _arm.armAngleCall - _arm.armAnglePrev;
    _arm.armAnglePrev = _arm.armAngleCall;
    _arm.armAngleSlow += (_arm.armAngleCall - _arm.armAngleSlow) / 100;
    _arm.armAngle = _arm.armAngleCall - _arm.armAngleOffset;

    //--------------------------------------------- LIMIT ERROR
    if (_shared.state == S_PLAYING) {
      if (_arm.armAngleCall > 0.95) {
        _shared.setError(E_ARMANGLE_LIMIT_POS);
        _arm.needleUp();
        _shared.setState(S_HOME);
      }
      if (_arm.armAngleCall < -0.95) {
        _shared.setError(E_ARMANGLE_LIMIT_NEG);
        _arm.needleUp();
        _shared.setState(S_HOME);
      }
    }

    sensorPosition = (position - CART_SENSOR_OFFSET) - trackOffset;

    stateUpdate();

    _Dcomp *= 0.999;
    _Dcomp += limitFloat(_arm.armAngleDiff * D, -CART_MAX_SPEED, CART_MAX_SPEED); // to prevent oscillation
    realPosition = position + _Dcomp;

    if (offCenterCompensation) {
      _offCenterCompFilter +=  (_speedcomp->cartFourierFilter - _offCenterCompFilter) / 4;
      realPosition += _offCenterCompFilter;
    }

    _motorPos = (realPosition + _offset) * _mm2step;

    if (_motorEnable) {
      pwmStepper(-_motorPos, CART_STEPPER_AP_PIN, CART_STEPPER_AN_PIN, CART_STEPPER_BP_PIN, CART_STEPPER_BN_PIN, true);
    } else {
      pwmDisableStepper(CART_STEPPER_AP_PIN, CART_STEPPER_AN_PIN, CART_STEPPER_BP_PIN, CART_STEPPER_BN_PIN);
    }

    if (_shared.state == S_PLAYING && _arm.isNeedleDown()) { // cart pos filter for display
      float div = position - positionFilter;
      if (div < 0) {
        positionFilter += (div) / 1000;
      }
    } else {
      positionFilter = position;
    }

    _motorEnable = true;

    if (graphicData) {
      printGraphicData();
    } else {
      _headerShown = false;
    }
  } // _interval.tick()
} // func()

void Cart::stateUpdate() {
  if (_shared.state == S_HOME) {
    _arm.centerArmAngle();
    _motorEnable = false;
    repeat = false;
    return;
  }

  if (_shared.state == S_STOPPING) {
    if (_arm.needleUp() && decelerate()) {
      _shared.setState(S_HOMING);
    }
    return;
  }


  if (_shared.state == S_HOMING || _shared.state == S_HOMING_BEFORE_PLAYING || _shared.state == S_HOMING_BEFORE_CLEANING) {
    // wait for stepper to stop shaking after turning on
    if (_shared.stateChangedInterval.duration() < 300) {
      return;
    }

    float speed = mapFloat(_arm.armAngleCall, 0.75, 0.5, 0, CART_MAX_SPEED);
    speed = limitFloat(speed, (CART_MAX_SPEED / 10), CART_MAX_SPEED);
    bool arrived = movetoPosition(-150, speed);
    // bool arrived = movetoPosition(-150, CART_MAX_SPEED);

    if (arrived) {
      _offset -= CART_PARK - position;
      position = CART_PARK;
      
      if (_shared.error == E_HOMING_FAILED){
        _shared.setError(E_HOMING_FAILED);
        _shared.setState(S_PARKING);
      } else {
        _shared.setError(E_HOMING_FAILED);
        _shared.setState(S_HOMING_FAILED);
      }
      return;
    }

    if (_arm.armAngleCall > 0.75) { //75 //-800 //-1000
      // LOG_DEBUG("cart.cpp", "[stateUpdate] Home diff: " + String(CART_PARK - realPosition) + " realPosition: " + String(realPosition) + " CART_PARK: " + String(CART_PARK) + " Dcomp: " + String(_Dcomp));
      LOG_DEBUG("cart.cpp", "[stateUpdate] Home diff: " + String(CART_PARK - realPosition) + " realPosition: " + String(realPosition));
      // Serial.println("Home diff: " + String(CART_PARK - realPosition) + " realPosition: " + String(realPosition));
      
      _offset -= CART_PARK - realPosition;
      position = CART_PARK;
      _Dcomp = 0;

      emergencyStop();
      _speedcomp->clearCompSamples(); // nice moment to stop

      if (_shared.state == S_HOMING_BEFORE_PLAYING) {
        _shared.setState(S_GOTO_RECORD_START);
      } else if (_shared.state == S_HOMING_BEFORE_CLEANING) {
        _shared.setState(S_NEEDLE_CLEAN);
      } else {
        _shared.setState(S_PARKING);
      }
      return;
    }
    return;
  }


  if (_shared.state == S_PARKING) {
    if (movetoPosition(CART_HOME, CART_MAX_SPEED)) {
      // if (_shared.stateChangedInterval.duration() < 2000) {
        _shared.setState(S_HOME);
      // }
    }
    _Dcomp = 0;
    // _Dcomp *= 0.98;
    return;
  }


  if (_shared.state == S_HOMING_FAILED) {
    if (movetoPosition(CART_HOME + 10, CART_MAX_SPEED)) {
      if (_shared.stateChangedInterval.duration() < 2000) {
        _arm.centerArmAngle();
        return;
      }
      _shared.setState(S_HOMING);
    } 
    return;
  }


  if (_shared.state == S_BAD_ORIENTATION) {
    _arm.needleUp();
    _motorEnable = false;
    return;
  }


  if (_shared.state == S_GOTO_RECORD_START) {
    if (_shared.firstTimeStateChanged()) {
      _scanner.recordStart = 1000;
    }

    if (!_scanner.recordPresent && sensorPosition > CART_RECORD_END + 1) { // record present?
      float recordDiaInch = (sensorPosition / 25.4) * 2;

      if (recordDiaInch < 6) { // stop when smaller than 6"
        // LOG_DEBUG("cart.cpp", "[stateUpdate] No record? RecordDiameter: " + String(recordDiaInch));
        Serial.println("No record? RecordDiameter: " + String(recordDiaInch));
        _plateau.stop();
        return;
      } else if (recordDiaInch < 9) {
        // LOG_DEBUG("cart.cpp", "[stateUpdate] RecordDiameter: " + String(recordDiaInch) + " : ±7\" ");
        Serial.println("RecordDiameter: " + String(recordDiaInch) + " : ±7\" ");
        _plateau.setRpm(RPM_45);
        _scanner.recordStart = CART_7INCH_START;
        _scanner.setTracksAs7inch();
      } else if (recordDiaInch < 11) { 
        // LOG_DEBUG("cart.cpp", "[stateUpdate] RecordDiameter: " + String(recordDiaInch) + " : ±10\" ");
        Serial.println("RecordDiameter: " + String(recordDiaInch) + " : ±10\" ");
        _plateau.setRpm(RPM_33);
        _scanner.recordStart = CART_10INCH_START;
        _scanner.check();
      } else {
        // LOG_DEBUG("cart.cpp", "[stateUpdate] RecordDiameter: " + String(recordDiaInch) + " : ???\" ");
        Serial.println("RecordDiameter: " + String(recordDiaInch) + " : ???\" ");
        _scanner.recordStart = sensorPosition;
        // _plateau.setRpm(RPM_33);
      }
      targetTrack = _scanner.recordStart;
      _shared.setState(S_PLAY_TILL_END);
      return;
    }

    // when arrived at cart endrange
    if (movetoPosition(CART_12INCH_START, CART_MAX_SPEED)) { 
      _scanner.recordStart = CART_12INCH_START;
      targetTrack = _scanner.recordStart;
      // LOG_DEBUG("cart.cpp", "[stateUpdate] RecordDiameter: 12\" ");
      Serial.println("RecordDiameter: 12\" ");
      _plateau.setRpm(RPM_33);
      _scanner.check();

      _shared.setState(S_PLAYING);
      return;
    }
    return;
  }

  if (_shared.state == S_PLAY_TILL_END) {
    if (movetoPosition(targetTrack, CART_MAX_SPEED)) {
      _shared.setState(S_PLAYING);
      return;
    }
    return;
  }


  //  ================================================================
  //      PLAY
  //  ================================================================
  if (_shared.state == S_PLAYING) {
    if (_shared.stateChangedInterval.duration() < 1000) {
      _arm.centerArmAngle();
      movedForwardInterval.reset();
      return;
    }

    // if (_arm.isNeedleDownFor(3000)) { // error 3 fix i think
    //   movedForwardInterval.reset(); // reset timer to prevent error 3
    // }

    if (_arm.needleDown()) {
      //---------------------------------------- transport calculations
      _newPosition = position + limitFloat(_arm.armAngle * P, -3, 3);
      _newPosition = limitFloat(_newPosition, 0, _scanner.recordStart);
      movetoPosition(_newPosition, CART_MAX_SPEED);
      
      //---------------------------------------- events during playing
      if (realPosition <= CART_RECORD_END) {
        // LOG_NOTICE("cart.cpp", "[stateUpdate] Cart reached limit!");
        Serial.println("Cart reached limit!");
        stopOrRepeat();
        return;
      }
      if (realPosition < positionFilter - 3) {
        // LOG_NOTICE("cart.cpp", "[stateUpdate] Run-out groove?");
        Serial.println("Run-out groove?");
        stopOrRepeat();
        return;
      }
      if (realPosition > positionFilter + 2.5) {
        _shared.setError(E_NEEDLE_MOVE_BACKWARDS);
        _plateau.stop();
        return;
      }

      if (_speedcomp->trackSpacing > 0.01) {
        movedForwardInterval.reset();
      } else if (movedForwardInterval.duration() > 4000) {
        if (position < 60 ){ // run-out groove? (54mm seems the farest from the middle
          stopOrRepeat();
          return;
        } else {
          _shared.setError(E_NEEDLE_DIDNT_MOVE); // cart didn't move for a while
          movedForwardInterval.reset(); // reset time to prevent another trigger
          gotoTrack(position - 0.25); // move cart 0.5mm inside to skip the skip
          return;
        }
      } 

      if (_shared.puristMode) {
        if ((_speedcomp->wow < 0.15) || _arm.isNeedleDownFor(10000) ){
          // LOG_NOTICE("cart.cpp", "[stateUpdate] Seems to runs ok");
          Serial.println("Seems to runs ok");
          _shared.puristMode = false;
          Serial.println("PURIST MODE: OFF");
        // gotoRecordStart();
          gotoTrack(targetTrack);
        }
      }    
    }
    return;
  }


  //  ================================================================
  //      TRACKS & SKIPPING
  //  ================================================================
  if (_shared.state == S_GOTO_TRACK) {
    if (_arm.needleUp()) {
      if (movetoPosition(targetTrack, CART_MAX_SPEED)) {
        _shared.setState(S_PLAYING);
        return;
      }
    }
    return;
  }

  if (_shared.state == S_SKIP_FORWARD) {
    if (_arm.needleUp()) {
      movetoPosition(CART_RECORD_END, CART_MAX_SPEED / 4);
    }
    targetTrack = position; // to clean display
  }

  if (_shared.state == S_SKIP_REVERSE) {
    if (_arm.needleUp()) {
      movetoPosition(_scanner.recordStart, CART_MAX_SPEED / 4);
    }
    targetTrack = position; // to clean display
  }

  if (_shared.state == S_RESUME_AFTER_SKIP) {
    if (movetoPosition(targetTrack, CART_MAX_SPEED / 4)) { 
      _shared.setState(S_PLAYING);
      return;
    }
    return;
  }

  if (_shared.state == S_PAUSE) {
    if (_arm.needleUp()) {
      movetoPosition(targetTrack, CART_MAX_SPEED);
    }
    return;
  }

  if (_shared.state == S_NEEDLE_CLEAN) {
    if (movetoPosition(CART_CLEAN_POS, CART_MAX_SPEED)) {
      _arm.needleDown();
    }

    if (sensorPosition > CART_RECORD_END + 2 && sensorPosition < CART_RECORD_END + 12 && _scanner.recordPresent) {
      // record present? stop!
      // LOG_ALERT("cart.cpp", "[stateUpdate] Cannot clean needle; Record present?");
      Serial.println("Cannot clean needle; Record present? Clean record instead");
      _shared.setState(S_RECORD_CLEAN);
      _plateau.motorStart();
      _plateau.setRpm(RPM_33);
    }
    return;
  }

  if (_shared.state == S_RECORD_CLEAN) {
    decelerate();
    if (!_scanner.recordPresent) {
      // LOG_ALERT("cart.cpp", "[stateUpdate] Record removed!");
      Serial.println("Record removed!");
      _plateau.stop();
    }
    return;
  }

  if (_shared.state == S_CALIBRATE) {
    if (_shared.stateChangedInterval.duration() < 100) {
      targetTrack = CART_CLEAN_POS;
    }
    if (movetoPosition(targetTrack, CART_MAX_SPEED)) {
      // Nothing      
    }
    return;
  }
} // stateUpdate()

void Cart::gotoNextTrack() {
  LOG_DEBUG("cart.cpp", "[gotoNextTrack]");

  float pos = positionFilter;

  if (_shared.state == S_GOTO_TRACK) {
    pos = targetTrack;
  }

  int track = _scanner.trackCount - 1;
  if (track <= 0) {
    stopOrRepeat();
    return;
  }

  while (pos - 2 <= _scanner.tracks[track]) { // 2mm offset to prevent repeating the same track
    track--;
    if (track <= 0) {
      stopOrRepeat();
      return;
    }
  }
  gotoTrack(_scanner.tracks[track]);
} // gotoNextTrack()

void Cart::gotoPreviousTrack() {
  LOG_DEBUG("cart.cpp", "[gotoPreviousTrack]");

  float pos = positionFilter;

  if (_shared.state == S_GOTO_TRACK) {
    pos = targetTrack;
  }

  int track = 0;
  while (pos + CART_BACKTRACK_OFFSET >= _scanner.tracks[track]) {
    track++;
    if (track > _scanner.trackCount - 1) {
      gotoRecordStart();
      return;
    }
  }

  if (_scanner.tracks[track] > _scanner.recordStart) {
    gotoRecordStart();
  }
  gotoTrack(_scanner.tracks[track]);
} // gotoPreviousTrack()

void Cart::gotoTrack(float pos) {
  LOG_DEBUG("cart.cpp", "[gotoTrack]");
  targetTrack = pos;
  // LOG_DEBUG("cart.cpp", "[gotoTrack] To position " + String(targetTrack));
  Serial.println("To position " + String(targetTrack));
  _shared.setState(S_GOTO_TRACK);
} // gotoTrack()

void Cart::gotoRecordStart() {
  LOG_DEBUG("cart.cpp", "[gotoRecordStart]");
  gotoTrack(_scanner.recordStart);
} // gotoRecordStart()

bool Cart::movetoPosition(float target, float spd) {
  _acceleration = 0;

  float togo = abs(target - position);
  int togoDirection = target - position > 0 ? 1 : -1;

  _distanceToStop = (_speed * _speed) / ( 2 * CART_ACCELERATION );
  int _distanceToStopDirection = _speed > 0 ? 1 : -1;

  if (isApprox(togo, 0, 0.01) && _distanceToStop < 0.1) {
    _speed = 0;
    return true;
  }

  if (_distanceToStop >= togo) {
    _acceleration = -CART_ACCELERATION * togoDirection;
  } else if (abs(_speed) < CART_MAX_SPEED) {
    _acceleration = CART_ACCELERATION * togoDirection;
  }

  position += _speed + (_acceleration / 2);
  _speed += _acceleration;
  _speed = limitFloat(_speed, -spd, spd);

  return false;
} // movetoPosition()

void Cart::stopOrRepeat() {
  LOG_DEBUG("cart.cpp", "[stopOrRepeat]");
  if (repeat) {
    gotoRecordStart();
  } else {
    _plateau.stop();
  }
} // stopOrRepeat()

void Cart::pause() {
  LOG_DEBUG("cart.cpp", "[pause]");
  if (_shared.state == S_PLAYING) {
    _shared.setState(S_PAUSE);
    targetTrack = position;
  } else if (_shared.state == S_PAUSE) {
    _shared.setState(S_PLAY_TILL_END);
  }
} // pause()

bool Cart::decelerate() {
  int direction = _speed > 0 ? 1 : -1;

  if(abs(_speed) < CART_ACCELERATION){
    _speed = 0;
    return true;
  }

  _acceleration = -CART_ACCELERATION * direction;
  position += _speed + (_acceleration / 2);
  _speed += _acceleration;
  return false;
} // decelerate()

void Cart::emergencyStop() {
  LOG_DEBUG("cart.cpp", "[emergencyStop]");
  _speed = 0;
} // emergencyStop()

void Cart::printGraphicData() {
  if (!_headerShown) {
    Serial.println("GRAPH_HEADER: DComp, ArmAngleRaw, ArmAngleCall");
    _headerShown = true;
  }
  Serial.print(_Dcomp, 5);
  Serial.print(", ");
  Serial.print(_arm.armAngleRaw);
  Serial.print(", ");
  Serial.print(_arm.armAngleCall, 2);
  Serial.println();
} // printGraphicData()

void Cart::info() {
  int padR = 25;
  Serial.println(padRight("CART_P", padR) + ": " + String(P, 5));
  Serial.println(padRight("CART_I", padR) + ": " + String(I, 5));
  Serial.println(padRight("CART_D", padR) + ": " + String(D, 5));
  Serial.println(padRight("CART_POSITION", padR) + ": " + String(position));
  Serial.println(padRight("CART_REAL_POSITION", padR) + ": " + String(realPosition));
  Serial.println();
} // info()
